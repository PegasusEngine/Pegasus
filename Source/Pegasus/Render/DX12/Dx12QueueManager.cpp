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
#include "GenericGpuResourcePool.h"
#include "../Common/InternalJobBuilder.h"
#include "../Common/JobTreeVisitors.h"

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

void Dx12QueueManager::GetD3DBarrier(const CanonicalJobPath::GpuBarrier& b, D3D12_RESOURCE_BARRIER& dx12Barrier) const
{
    //TODO: handle UAV barriers
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

    dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    auto& transitionBarrier = dx12Barrier.Transition;
    const Dx12Resource* dx12Resource = Dx12Resource::GetDx12Resource(b.resource);
    transitionBarrier.pResource = dx12Resource->GetD3D();
    transitionBarrier.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transitionBarrier.StateBefore = GetD3D12State(b.from, b.resource->GetStateId(), dx12Resource);
    transitionBarrier.StateAfter = GetD3D12State(b.to, b.resource->GetStateId(), dx12Resource);
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
    }

    mGlobalResourceStateTable = mDevice->GetResourceStateTable();
    mGlobalResourceStateDomain = mGlobalResourceStateTable->CreateDomain();
}

Dx12QueueManager::~Dx12QueueManager()
{
    for (int queueIt = 0; queueIt < (int)WorkType::Count; ++queueIt)
        mQueueContainers[queueIt].queue->Release();

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
    PG_ASSERT(!handle.isValid(mWorks.size()));
    if (!handle.isValid(mWorks.size()))
        return;

    GpuWork& work = mWorks[handle];
    PG_ASSERT(!work.submitted);

    if (!work.submitted)   
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

void Dx12QueueManager::TranspileList(const JobInstance* jobTable, const CanonicalJobPath& job, GpuList& gpuList)
{
    unsigned barriersCount = 0u;
    const CanonicalJobPath::GpuBarrier* barriers = job.GetBarriers(barriersCount);

    auto applyBarriers = [&](const unsigned*  indices, unsigned counts, std::vector<D3D12_RESOURCE_BARRIER>& outBarriers)
    {
        outBarriers.resize(counts);
        for (unsigned i = 0u; i < counts; ++i)
        {
            GetD3DBarrier(barriers[indices[i]], outBarriers[i]);
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
            },
            [&](const ComputeCmdData& d){
            },
            [&](const CopyCmdData& d){
                const Dx12Resource* srcRes = Dx12Resource::GetDx12Resource(d.src);
                const Dx12Resource* dstRes = Dx12Resource::GetDx12Resource(d.dst);
                if (srcRes != nullptr && dstRes != nullptr)
                {
                    gpuList.list->CopyResource(srcRes->GetD3D(), dstRes->GetD3D());
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

    gpuList.list->Close();
    
}

}
}
