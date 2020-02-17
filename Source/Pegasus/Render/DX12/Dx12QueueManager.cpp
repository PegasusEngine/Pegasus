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

static void getD3DBarrier(const CanonicalJobPath::GpuBarrier& b, D3D12_RESOURCE_BARRIER& dx12Barrier)
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

    dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    auto& transitionBarrier = dx12Barrier.Transition;
    const Dx12Resource* dx12Resource = Dx12Resource::GetDx12Resource(b.resource);
    transitionBarrier.pResource = dx12Resource->GetD3D();
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

    mGlobalResourceStateDomain = mDevice->GetResourceStateTable()->CreateDomain();
}

Dx12QueueManager::~Dx12QueueManager()
{
    for (int queueIt = 0; queueIt < (int)WorkType::Count; ++queueIt)
        mQueueContainers[queueIt].queue->Release();

    mDevice->GetResourceStateTable()->RemoveDomain(mGlobalResourceStateDomain);
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

GpuWorkResultCode Dx12QueueManager::CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobCounts)
{
    if (!handle.isValid(mWorks.size()))
        return GpuWorkResultCode::InvalidArgs;

    GpuWork& work = mWorks[handle];
    for (unsigned i = 0; i < jobCounts; ++i)
        AllocateList(WorkType::Graphics, work);

    for (unsigned i = 0; i < jobCounts; ++i)
        TranspileList(jobs[i], work.gpuLists[i]);

    return GpuWorkResultCode::Success;
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

void Dx12QueueManager::TranspileList(const CanonicalJobPath& job, GpuList& gpuList)
{
    unsigned barriersCount = 0u;
    const CanonicalJobPath::GpuBarrier* barriers = job.GetBarriers(barriersCount);

    auto applyBarriers = [&](const unsigned*  indices, unsigned counts)
    {
        for (unsigned i = 0u; i < counts; ++i)
        {
            const CanonicalJobPath::GpuBarrier& b = barriers[indices[i]];
            D3D12_RESOURCE_BARRIER dx12Barrier;
            getD3DBarrier(b, dx12Barrier);
        }
    };

    for (const CanonicalJobPath::Node& n : job.GetCmdList())
    {
        applyBarriers(n.preGpuBarrierIndices.data(), (unsigned)n.preGpuBarrierIndices.size());
        applyBarriers(n.postGpuBarrierIndices.data(), (unsigned)n.postGpuBarrierIndices.size());
    }
    
}

}
}
