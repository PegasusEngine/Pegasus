/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12QueueManager.cpp
//! \author Kleber Garcia
//! \date   July 19th 2018
//! \brief  Implementation of command queue shenanigans

#include "Dx12QueueManager.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12Resources.h"
#include "Dx12Fence.h"
#include "Dx12RDMgr.h"
#include "Dx12Pso.h"
#include "GenericGpuResourcePool.h"
#include "../Common/InternalJobBuilder.h"
#include "../Common/JobTreeVisitors.h"
#include "GenericGpuResourcePool.h"

#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{

static D3D12_COMMAND_LIST_TYPE GetCmdListType(Dx12QueueManager::WorkType workType)
{
    static D3D12_COMMAND_LIST_TYPE sCmdListTypes[(int)Dx12QueueManager::WorkType::Count] = {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_COPY
    };

    return sCmdListTypes[(unsigned)workType];
}

D3D12_RESOURCE_STATES Dx12QueueManager::GetD3D12State(const LocationGpuState& state, unsigned stateId, const Dx12Resource* dx12Resource) const
{
    static D3D12_RESOURCE_STATES sDx12States[(int)ResourceGpuState::Count] =
    {
        D3D12_RESOURCE_STATE_COMMON, //Default,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,//Uav,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,//UavWrite, unused
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//Srv,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,//Cbv,
        D3D12_RESOURCE_STATE_RENDER_TARGET,//Rt,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,//Ds,
        D3D12_RESOURCE_STATE_COPY_SOURCE,//CopySrc,
        D3D12_RESOURCE_STATE_COPY_DEST//CopyDst,
    };

    ResourceGpuState resState = state.gpuState;

    if (!state.location.isValid())
    {
        unsigned globalState = 0u;
        if (mGlobalResourceStateTable->GetState(mGlobalResourceStateDomain, stateId, globalState))
        {
            resState = (ResourceGpuState)globalState;
        }
        else
        {
            return dx12Resource->GetDefaultState();
        }
    }

    return sDx12States[(int)resState];
}

bool Dx12QueueManager::GetD3DBarrier(const CanonicalJobPath::GpuBarrier& b, D3D12_RESOURCE_BARRIER& dx12Barrier) const
{
	dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

    switch (b.timing)
    {
    case CanonicalJobPath::BarrierTiming::Begin:
        dx12Barrier.Flags |= D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;
        break;
    case CanonicalJobPath::BarrierTiming::End:
        dx12Barrier.Flags |= D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
        break;
	case CanonicalJobPath::BarrierTiming::BeginAndEnd:
    default:
		break;
    };

    const Dx12Resource* dx12Resource = Dx12Resource::GetDx12Resource(b.resource);
    if (b.to.gpuState == ResourceGpuState::UavWrite &&
        (b.timing == CanonicalJobPath::BarrierTiming::End || b.timing == CanonicalJobPath::BarrierTiming::BeginAndEnd))
    {
        dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        auto& uavBarrier = dx12Barrier.UAV;
        uavBarrier.pResource = dx12Resource->GetD3D();
		return true;
    }
    else
    {
        dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        auto& transitionBarrier = dx12Barrier.Transition;
        transitionBarrier.pResource = dx12Resource->GetD3D();
        transitionBarrier.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        transitionBarrier.StateBefore = GetD3D12State(b.from, b.resource->GetStateId(), dx12Resource);
        transitionBarrier.StateAfter = GetD3D12State(b.to, b.resource->GetStateId(), dx12Resource);
		return (transitionBarrier.StateBefore != transitionBarrier.StateAfter);
    }
}

Dx12QueueManager::Dx12QueueManager(Pegasus::Alloc::IAllocator* allocator, Dx12Device* device)
: mDevice(device), mAllocator(allocator)
{
    for (int queueIt = 0u; queueIt < (int)WorkType::Count; ++queueIt)
    {
        QueueContainer& qcontainer = mQueueContainers[queueIt];

        D3D12_COMMAND_QUEUE_DESC qDesc = {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            D3D12_COMMAND_QUEUE_FLAG_NONE,
            0 /*node mask*/
        };

        qDesc.Type = GetCmdListType((Dx12QueueManager::WorkType)queueIt);
        DX_VALID_DECLARE(mDevice->GetD3D()->CreateCommandQueue(&qDesc, __uuidof(qcontainer.queue), &((void*)qcontainer.queue)));

		qcontainer.fence = D12_NEW(allocator, "queuFence") Dx12Fence(device, qcontainer.queue);
        qcontainer.uploadPool = D12_NEW(allocator, "uploadPool") GpuUploadPool(device, qcontainer.queue, 32 * 1024 * 1024);
        qcontainer.tablePool = D12_NEW(allocator, "tablePool") GpuDescriptorTablePool(device, qcontainer.queue, 2000u, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        qcontainer.uploadPool->BeginUsage();
        qcontainer.tablePool->BeginUsage();
    }

    mGlobalResourceStateTable = mDevice->GetResourceStateTable();
    mGlobalResourceStateDomain = mGlobalResourceStateTable->CreateDomain();
}

Dx12QueueManager::~Dx12QueueManager()
{
	for (int queueIt = 0; queueIt < (int)WorkType::Count; ++queueIt)
	{
		auto& qcontainer = mQueueContainers[queueIt];
		qcontainer.fence->WaitOnCpu(qcontainer.fence->GetValue());
		qcontainer.queue->Release();
		D12_DELETE(mAllocator, qcontainer.fence);
		qcontainer.uploadPool->EndUsage();
		qcontainer.tablePool->EndUsage();
        D12_DELETE(mAllocator, qcontainer.uploadPool);
        D12_DELETE(mAllocator, qcontainer.tablePool);
	}

    mGlobalResourceStateTable->RemoveDomain(mGlobalResourceStateDomain);
}

Pegasus::Render::GpuWorkHandle Dx12QueueManager::AllocateWork()
{
    Pegasus::Render::GpuWorkHandle newHandle;
    if (!mFreeHandles.empty())
    {
        newHandle = mFreeHandles.back();
        mFreeHandles.pop_back();
    }
    else
    {
        newHandle = mWorks.size();
        mWorks.emplace_back();
    }

    return newHandle;
}

void Dx12QueueManager::DestroyWork(GpuWorkHandle handle)
{
    PG_ASSERT(handle.isValid(mWorks.size()));
    mWorks[handle] = GpuWork();
    mFreeHandles.push_back(handle);
}

GpuWorkResultCode Dx12QueueManager::CompileWork(GpuWorkHandle handle, const RootJob& rootJob, const CanonicalJobPath* jobs, unsigned jobCounts)
{
    if (!handle.isValid(mWorks.size()))
        return GpuWorkResultCode::InvalidArgs;

    GpuWork& work = mWorks[handle];
    work.jobBuilder = rootJob.GetParent();

    for (unsigned i = 0; i < jobCounts; ++i)
    {
        AllocateList(WorkType::Graphics, work);
        FlushEndpointBarriers(jobs[i], work);
    }

    for (unsigned i = 0; i < jobCounts; ++i)
        TranspileList(work.jobBuilder->jobTable.data(), jobs[i], work.gpuLists[i]);

    return GpuWorkResultCode::Success;
}

void Dx12QueueManager::SubmitWork(GpuWorkHandle handle)
{
    PG_ASSERT(handle.isValid(mWorks.size()));
    if (!handle.isValid(mWorks.size()))
        return;

    GpuWork& work = mWorks[handle];
    PG_ASSERT(!work.submitted);

    if (work.submitted)   
        return;

    auto& queueContainer = mQueueContainers[(int)WorkType::Graphics];
    std::vector<ID3D12CommandList*> submitList;
    submitList.reserve(work.gpuLists.size());
    work.submitted = true;
    for (auto gpuList : work.gpuLists)
    {
        submitList.push_back(gpuList.list);
        
        //free up pending lists right after submission
        queueContainer.freeLists.push_back(gpuList.list);
    }

    for (auto b : work.statesEndpoint)
    {
        mGlobalResourceStateTable->StoreState(
            mGlobalResourceStateDomain, b.resource->GetStateId(),
           (unsigned)b.to.gpuState);
    }

    queueContainer.queue->ExecuteCommandLists(
        (UINT)submitList.size(),
        submitList.data()
    );

	work.parentFence = queueContainer.fence;
    work.fenceVal = queueContainer.fence->Signal();
}

void Dx12QueueManager::WaitOnCpu(GpuWorkHandle handle)
{
    PG_ASSERT(handle.isValid(mWorks.size()));
    if (!handle.isValid(mWorks.size()))
        return;

    GpuWork& work = mWorks[handle];
    PG_ASSERT(work.submitted);

    work.parentFence->WaitOnCpu(work.fenceVal);
}

void Dx12QueueManager::AllocateList(WorkType workType, Dx12QueueManager::GpuWork& work)
{
    GpuList newGpuList;
    newGpuList.type = workType;
    auto listType = GetCmdListType(workType);
    auto& queueContainer = mQueueContainers[(int)workType];

    if (!queueContainer.freeAllocators.empty())
    {
		newGpuList.allocator = queueContainer.freeAllocators.back();
        queueContainer.freeAllocators.pop_back();
    }
    else
    {
        DX_VALID_DECLARE(mDevice->GetD3D()->CreateCommandAllocator(listType, __uuidof(newGpuList.allocator), (void**)&newGpuList.allocator));
    }
    
    if (!queueContainer.freeLists.empty())
    {
		newGpuList.list = queueContainer.freeLists.back();
        queueContainer.freeLists.pop_back();
		newGpuList.list->Reset(newGpuList.allocator, nullptr);
    }
    else
    {
        DX_VALID_DECLARE(mDevice->GetD3D()->CreateCommandList(0u, listType, newGpuList.allocator, nullptr, __uuidof(newGpuList.list), (void**)(&newGpuList.list)));
    }
    
    work.gpuLists.push_back(newGpuList);
}

void Dx12QueueManager::FlushEndpointBarriers(const CanonicalJobPath& job, GpuWork& work)
{
    unsigned barriersSz = 0u;
    const CanonicalJobPath::GpuBarrier* barriers = job.GetBarriers(barriersSz);
    for (unsigned i = 0; i < barriersSz; ++i)
    {
        const CanonicalJobPath::GpuBarrier& b = barriers[i];
        if ( (b.timing == CanonicalJobPath::BarrierTiming::End || b.timing == CanonicalJobPath::BarrierTiming::BeginAndEnd) &&
            b.to.location.isValid()
            )
        {
            work.statesEndpoint.push_back(b);
        }
    }
}

template<typename GenericCmdData, bool IsCompute>
static void SetDx12ResourceTables(
    Dx12Device* device,
    const JobInstance& jobInstance,
    const GenericCmdData& cmdData,
    GpuDescriptorTablePool& tablePool,
    ID3D12GraphicsCommandList* list)
{
    if (jobInstance.pso == nullptr)
        return;

    const Dx12Pso* dx12Pso = static_cast<const Dx12Pso*>(&(*jobInstance.pso));
        
    DescriptorTable cbv = {};
    DescriptorTable srvs = {};
    DescriptorTable uavs = {};

    unsigned cbvCounts = (unsigned)cmdData.cbuffer.size();

    unsigned uavCounts = 0u;
    for (ResourceTableRef t : jobInstance.uavTables)
        uavCounts += t != nullptr ? (unsigned)t->GetConfig().resources.size() : 0u;

    unsigned srvCounts = 0u;
    for (ResourceTableRef t : jobInstance.srvTables)
        srvCounts += t != nullptr ? (unsigned)t->GetConfig().resources.size() : 0u;

    DescriptorTable cbvSrvUavTables = tablePool.AllocateTable(cbvCounts + uavCounts + srvCounts);

    if (cbvCounts)
    {
        cbv = cbvSrvUavTables; 
        cbv.descriptorCounts = cbvCounts;
        for (unsigned it = 0; it < (unsigned)cmdData.cbuffer.size(); ++it)
        {
            if (cmdData.cbuffer[it] == nullptr)
                continue;

            const auto* dx12Buffer = static_cast<const Dx12Buffer*>(&(*cmdData.cbuffer[it]));
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = dx12Buffer->GetVA();
            cbvDesc.SizeInBytes = dx12Buffer->GetUploadSz();
            device->GetD3D()->CreateConstantBufferView(&cbvDesc, cbv.GetCpuHandle(it));
        }
        cbvSrvUavTables.Advance(cbv.descriptorCounts);
    }
    
    auto buildResDesc = [&](unsigned elementCounts, const std::vector<ResourceTableRef>& resTables, DescriptorTable& destTable)
    {
        if (elementCounts == 0u)
            return;
    
        destTable = cbvSrvUavTables;
        destTable.descriptorCounts = elementCounts;
        for (ResourceTableRef t : resTables)
        {
            if (t == nullptr)
                continue;

            const auto* dx12Table = static_cast<const Dx12ResourceTable*>(&(*t));
            const Dx12RDMgr::Table& precomputedTable = dx12Table->GetTable();
            device->GetD3D()->CopyDescriptorsSimple(
                precomputedTable.count,
                cbvSrvUavTables.cpuHandle,
                precomputedTable.baseHandle,
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
            );

            cbvSrvUavTables.Advance(precomputedTable.count);
        }
    };

    buildResDesc(uavCounts, jobInstance.uavTables, uavs);
    buildResDesc(srvCounts, jobInstance.srvTables, srvs);

    ID3D12DescriptorHeap* descHeaps[4] = {}; //heaps for srv/uav/cbv/sampler/rtv
    unsigned descHeapsCount = 0u;
    
    if (cbvCounts || uavCounts || srvCounts)
    {
        descHeaps[descHeapsCount++] = cbvSrvUavTables.ownerHeap;
    }

    //TODO: add sampler support here

    PG_ASSERT(descHeapsCount <= sizeof(descHeaps)/sizeof(descHeaps[0]));
    list->SetDescriptorHeaps(descHeapsCount, descHeaps);

    auto setTable = [&](UINT tableId, D3D12_GPU_DESCRIPTOR_HANDLE h)
    {
        if (IsCompute)
            list->SetComputeRootDescriptorTable(tableId, h);
        else
            list->SetGraphicsRootDescriptorTable(tableId, h);
    };

    //setting now srv/uav and cbv tables based on root signature
    UINT outCbvTableId = 0u;
    if (cbvCounts && dx12Pso->SpaceToTableId(0u, Dx12_ResCbv, outCbvTableId))
    {
        setTable(outCbvTableId, cbv.gpuHandle);
    }

    auto setManyTables = [&](unsigned elementCounts, Dx12ResType resType, const std::vector<ResourceTableRef>& resTables, DescriptorTable& descTable)
    {
        if (elementCounts == 0u)
            return;
    
        unsigned spaceId = 0u;
        unsigned resourceIt = 0u;
        for (ResourceTableRef t : resTables)
        {
            if (t != nullptr)
            {
                const auto* dx12Table = static_cast<const Dx12ResourceTable*>(&(*t));
                const Dx12RDMgr::Table& precomputedTable = dx12Table->GetTable();

                unsigned tableId = 0u;
                if (precomputedTable.count > 0u && dx12Pso->SpaceToTableId(spaceId, resType, tableId))
                {
                    setTable(tableId, descTable.GetGpuHandle(resourceIt));
                }

                resourceIt += precomputedTable.count;
            }

            ++spaceId;
        }
    };

    setManyTables(srvCounts, Dx12_ResSrv, jobInstance.srvTables, srvs);
    setManyTables(uavCounts, Dx12_ResUav, jobInstance.uavTables, uavs);
}

void Dx12QueueManager::TranspileList(const JobInstance* jobTable, const CanonicalJobPath& job, GpuList& gpuList)
{
    unsigned barriersCount = 0u;
    const CanonicalJobPath::GpuBarrier* barriers = job.GetBarriers(barriersCount);

    auto applyBarriers = [&](const unsigned*  indices, unsigned counts, std::vector<D3D12_RESOURCE_BARRIER>& outBarriers)
    {
        outBarriers.reserve(counts);
        for (unsigned i = 0u; i < counts; ++i)
        {
            D3D12_RESOURCE_BARRIER b;
			if (GetD3DBarrier(barriers[indices[i]], b))
				outBarriers.emplace_back(std::move(b));
        }
    };

    for (const CanonicalJobPath::Node& n : job.GetCmdList())
    {
        std::vector<D3D12_RESOURCE_BARRIER> preDx12Barriers;
        applyBarriers(n.preGpuBarrierIndices.data(), (unsigned)n.preGpuBarrierIndices.size(), preDx12Barriers);
        
        std::vector<D3D12_RESOURCE_BARRIER> postDx12Barriers;
        applyBarriers(n.postGpuBarrierIndices.data(), (unsigned)n.postGpuBarrierIndices.size(), postDx12Barriers);

    
        if (!preDx12Barriers.empty())
            gpuList.list->ResourceBarrier((UINT)preDx12Barriers.size(), preDx12Barriers.data());

        const JobInstance& instance = jobTable[n.jobHandle];
        
        std::visit(JobVisitor::overloaded {
            [&](const RootCmdData& d){
            },
            [&](const DrawCmdData& d){
                SetDx12ResourceTables<DrawCmdData, false>(
                    mDevice, instance, d,
                    *GetTablePool(), gpuList.list);
            },
            [&](const ComputeCmdData& d){
                if (instance.pso != nullptr)
                {
                    const Dx12Pso* pso = static_cast<const Dx12Pso*>(*(&instance.pso));
                    if (pso->GetD3DRootSignature() != nullptr && pso->GetD3DPso() != nullptr)
                    {
                        gpuList.list->SetComputeRootSignature(pso->GetD3DRootSignature());
                        gpuList.list->SetPipelineState(pso->GetD3DPso());
                        SetDx12ResourceTables<ComputeCmdData, true>(
                            mDevice, instance, d,
                            *GetTablePool(), gpuList.list);
						gpuList.list->Dispatch(d.x, d.y, d.z);
                    }
                }
                    
            },
            [&](const CopyCmdData& d){
                const Dx12Resource* srcRes = Dx12Resource::GetDx12Resource(d.src);
                const Dx12Resource* dstRes = Dx12Resource::GetDx12Resource(d.dst);
                if (srcRes != nullptr && dstRes != nullptr)
                {
                    gpuList.list->CopyResource(dstRes->GetD3D(), srcRes->GetD3D());
                }
            },
            [&](const DisplayCmdData& d){
            },
            [&](const GroupCmdData& d){
            }
        }, instance.data);

        if (!postDx12Barriers.empty())
            gpuList.list->ResourceBarrier((UINT)postDx12Barriers.size(), postDx12Barriers.data());
    }

    DX_VALID_DECLARE(gpuList.list->Close());
    
}

void Dx12QueueManager::GarbageCollect()
{
	for (int queueIt = 0; queueIt < (int)WorkType::Count; ++queueIt)
	{
        mQueueContainers[queueIt].uploadPool->EndUsage();
        mQueueContainers[queueIt].uploadPool->BeginUsage();

        mQueueContainers[queueIt].tablePool->EndUsage();
        mQueueContainers[queueIt].tablePool->BeginUsage();
    }
}

}
}
