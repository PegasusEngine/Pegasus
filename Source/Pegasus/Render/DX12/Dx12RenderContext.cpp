/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RenderContext.h
//! \author Kleber Garcia
//! \date   March 4 2019
//! \brief  Implementation of render context

#include "Dx12RenderContext.h"
#include "Dx12Fence.h"
#include "Dx12Defs.h"
#include "Dx12Device.h"
#include <unordered_map>

namespace Pegasus
{
namespace Render
{

struct ResourceDescriptor
{
    UINT guid = 0;
    UINT count = 0;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
};

struct Dx12DrawState
{
    std::vector<Dx12ResourceTableRef> tables;
    Dx12PsoRef pso;
    
    void Reset()
    {
        pso = nullptr;
        tables.clear();
    }  
};

class Dx12FramePayload
{
public:
    Dx12FramePayload();
    ~Dx12FramePayload(){}
    void IncreaseUsedRD(Dx12RDMgr::TableType tableType, UINT rdAllocated) { mRDAllocated[tableType] += rdAllocated; }
    UINT GetUsedRD(Dx12RDMgr::TableType tableType) const { return mRDAllocated[tableType]; }
    void StoreTable(const Dx12RDMgr::Table& table, const ResourceDescriptor& rd);
    bool RetrieveTable(const Dx12RDMgr::Table& table, ResourceDescriptor& rd);
    void Reset();

private:
    UINT mRDAllocated[Dx12RDMgr::TableTypeMax];
    std::unordered_map<UINT, ResourceDescriptor> mTableCache;
};

Dx12FramePayload::Dx12FramePayload()
{
    Reset();
}

void Dx12FramePayload::Reset()
{
    for (auto& rdAllocated : mRDAllocated)
        rdAllocated = 0u;

    mTableCache.clear();
}

void Dx12FramePayload::StoreTable(const Dx12RDMgr::Table& table, const ResourceDescriptor& rd)
{
    PG_ASSERTSTR(table.guid != 0, "Table was not initialized?");
    PG_ASSERT(rd.guid == table.guid);
    mTableCache[table.guid] = rd;
}

bool Dx12FramePayload::RetrieveTable(const Dx12RDMgr::Table& table, ResourceDescriptor& rd)
{
    auto it = mTableCache.find(table.guid);
    if (it != mTableCache.end())
    {
        PG_ASSERT(table.guid == rd.guid);
        rd = it->second;
    }
    
    return false;
}


Dx12RenderContext::Dx12RenderContext(Dx12Device* device, UINT maxFrames, ID3D12CommandQueue* queue)
: mQueue(queue), mMaxFrames(maxFrames), mFrameId(0)
{
    mQueue->AddRef();
    mDevice = device;
    mFence = D12_NEW(device->GetAllocator(), "Dx12Fence") Dx12Fence(device, mQueue);
    mFrameStates = new FrameState[maxFrames];
    for (UINT f = 0; f < maxFrames; ++f)
    {
        auto& frameState = mFrameStates[f];
        DX_VALID_DECLARE(
            mDevice->GetD3D()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
           __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&frameState.cmdListAllocator)));

        frameState.payload = D12_NEW(device->GetAllocator(), "Dx12FramePayload") Dx12FramePayload;
        frameState.state = D12_NEW(device->GetAllocator(), "Dx12FrameState") Dx12DrawState;
    }

	DX_VALID_DECLARE(
        mDevice->GetD3D()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        CurrFrame().cmdListAllocator,
        nullptr, //Null PSO
        __uuidof(ID3D12GraphicsCommandList),
        reinterpret_cast<void**>(&mCmdList))
    );

	DX_VALID(mCmdList->Close());

    D3D12_DESCRIPTOR_HEAP_TYPE d3dDescHeapsTypes[] = {D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_TYPE_RTV};
    for (auto& d3dDescHeapType : d3dDescHeapsTypes)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        desc.Type = d3dDescHeapType;
        desc.NodeMask = 0;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        switch(d3dDescHeapType)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            desc.NumDescriptors = 4096;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            desc.NumDescriptors = 1024;
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            desc.NumDescriptors = 1024;
            break;
        default:
            break;
        }
    
        auto& container = mRDTableHeaps[Dx12RDMgr::GetTableType(d3dDescHeapType)];
        container.incrSize = device->GetD3D()->GetDescriptorHandleIncrementSize(d3dDescHeapType);
        container.totalSize = container.sizeLeft = desc.NumDescriptors;
        DX_VALID(device->GetD3D()->CreateDescriptorHeap(
            &desc, __uuidof(ID3D12DescriptorHeap),
            reinterpret_cast<void**>(&container.heap)));
        container.cpuDescStart = container.heap->GetCPUDescriptorHandleForHeapStart();
        container.gpuDescStart = container.heap->GetGPUDescriptorHandleForHeapStart();
    }
}

bool Dx12RenderContext::allocateGpuTable(const Dx12RDMgr::Table& table, ResourceDescriptor& rd)
{
    auto& currFrame = CurrFrame();
    if (currFrame.payload->RetrieveTable(table, rd))
        return true;

    PG_ASSERT(table.tableType >= 0u && table.tableType < Dx12RDMgr::TableTypeMax);
    auto& container = mRDTableHeaps[table.tableType];
    PG_ASSERT(container.heap);
    if (container.heap)
    {
        UINT paddingSize = 0;
        UINT newOffset = container.offset;

        if ((newOffset + table.count) > container.totalSize)
        {
            paddingSize = container.totalSize - newOffset;
            newOffset = 0;
        }

        UINT allocationSize = (table.count + paddingSize);
        UINT allocationOffset = newOffset;
        if (allocationSize > container.sizeLeft)
        {
            PG_FAILSTR("Ran out of GPU descriptor table slots. Table type %d", table.tableType);
        }
        else
        {
            rd.guid = table.guid;
            rd.count = table.count;
            rd.gpuHandle = container.gpuDescStart;
            rd.cpuHandle = container.cpuDescStart;
            rd.gpuHandle.ptr += container.incrSize * allocationOffset;
            rd.cpuHandle.ptr += container.incrSize * allocationOffset;

            container.offset = (newOffset + table.count) % container.totalSize;
            container.sizeLeft -= allocationSize;

            currFrame.payload->IncreaseUsedRD(table.tableType, allocationSize);
            currFrame.payload->StoreTable(table, rd);
            mDevice->GetD3D()->CopyDescriptorsSimple(table.count, rd.cpuHandle, table.baseHandle, Dx12RDMgr::GetHeapType(table.tableType));
            return true;
        }
    }

    return false;
}

void Dx12RenderContext::flushDrawState()
{
    auto* state = CurrFrame().state;

    if (state->pso == nullptr)
    {
        PG_LOG('ERR_', "Must set a pso before calling draw!");
        return;
    }

    mCmdList->SetGraphicsRootSignature(state->pso->GetD3DRootSignature());
    mCmdList->SetPipelineState(state->pso->GetD3DPso());

    for (UINT spaceId = 0; spaceId < (UINT)state->tables.size(); ++spaceId)
    {
        auto& table = state->tables[spaceId];
        UINT tableId;
        if (!state->pso->SpaceToTableId(spaceId, table->GetConfig().type == ResourceTableType_Srv ? Dx12_ResSrv : Dx12_ResUav, tableId))
        {
            PG_LOG('ERR_', "No space id found %d.", spaceId);
            continue;
        }

        ResourceDescriptor rd;
        allocateGpuTable(table->mTable, rd);
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        barriers.reserve(10);
        for (auto& res : table->GetResources())
        {
            const auto targetState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            auto* dx12Resource = Dx12Resource::GetDx12Resource(&(*res));
            if (dx12Resource->GetState(0u) != targetState)
            {
				D3D12_RESOURCE_BARRIER barrier;
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = const_cast<Dx12Resource*>(dx12Resource)->GetD3D();
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = dx12Resource->GetState(0u);
                barrier.Transition.StateAfter = targetState;
				barriers.push_back(barrier);
            }
        }

        if (!barriers.empty())
            mCmdList->ResourceBarrier((UINT)barriers.size(), barriers.data());

        mCmdList->SetGraphicsRootDescriptorTable(tableId, rd.gpuHandle);
    }
}

Dx12RenderContext::~Dx12RenderContext()
{
    Flush();
    D12_DELETE(mDevice->GetAllocator(), mFence);
    mQueue->Release();
    for (UINT f = 0; f < mMaxFrames; ++f)
    {
        mFrameStates[f].cmdListAllocator = nullptr;
        D12_DELETE(mDevice->GetAllocator(), mFrameStates[f].payload);
        D12_DELETE(mDevice->GetAllocator(), mFrameStates[f].state);
    }
    delete [] mFrameStates;
}

void Dx12RenderContext::Flush()
{
    for (UINT f = 0; f < mMaxFrames; ++f)
    {
        mFence->WaitOnCpu(mFrameStates[(mFrameId + f) % mMaxFrames].fenceVal);
    }
    mFrameId = 0;
}

void Dx12RenderContext::BeginFrame()
{
    auto& frame = CurrFrame();
    mFence->WaitOnCpu(frame.fenceVal);
    for (UINT tableType = 0; tableType < (UINT)Dx12RDMgr::TableTypeMax; ++tableType)
    {
        mRDTableHeaps[tableType].sizeLeft += frame.payload->GetUsedRD((Dx12RDMgr::TableType)tableType);
    } 
    frame.payload->Reset();
    frame.state->Reset();

	DX_VALID_DECLARE(frame.cmdListAllocator->Reset());
    DX_VALID(mCmdList->Reset(frame.cmdListAllocator, nullptr));
}

void Dx12RenderContext::SetResourceTable(Dx12ResourceTableRef table, UINT space)
{
    auto& frame = CurrFrame();
    if (space >= (UINT)frame.state->tables.size())
        frame.state->tables.resize(space + 1, nullptr);

    frame.state->tables[space] = table;

}

void Dx12RenderContext::SetPso(Dx12PsoRef pso)
{
    CurrFrame().state->pso = pso;
}


void Dx12RenderContext::EndFrame()
{
    auto& frame = CurrFrame();
    frame.fenceVal = mFence->Signal();
    ++mFrameId;
}

}
}
