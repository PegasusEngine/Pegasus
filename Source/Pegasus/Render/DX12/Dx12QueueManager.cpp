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
#include "../Common/JobTreeVisitors.h"

#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{

Dx12QueueManager::Dx12QueueManager(Alloc::IAllocator* allocator, Dx12Device* device)
: mDevice(device->GetD3D()), mAllocator(allocator)
{
    mDevice->AddRef();

    for (int queueIt = 0u; queueIt < (int)WorkType::WorkTypeCount; ++queueIt)
    {
        D3D12_COMMAND_QUEUE_DESC qDesc = {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            D3D12_COMMAND_QUEUE_FLAG_NONE,
            0 /*node mask*/
        };

        WorkType workType = (WorkType)queueIt;
        switch (queueIt)
        {
        case WorkType::Graphics:
            qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        case WorkType::Compute:
            qDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;
        case WorkType::Copy:
        defaultP:
            qDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            break;
        }
        auto& qcontainer = mQueueContainers[queueIt];
        DX_VALID_DECLARE(mDevice->CreateCommandQueue(&qDesc, __uuidof(qcontainer.queue), &((void*)qcontainer.queue)));
    }
}

Dx12QueueManager::~Dx12QueueManager()
{
    for (int queueIt = 0; queueIt < (int)WorkTypeCount; ++queueIt)
        mQueueContainers[queueIt].queue->Release();
    mDevice->Release();
}

GpuWorkHandle Dx12QueueManager::AllocateWork()
{
    GpuWorkHandle newHandle;
    if (!mFreeHandles.empty())
    {
        newHandle = mFreeHandles.back();
        mFreeHandles.pop_back();
    }
    else
    {
        newHandle = mWork.size();
        mWork.emplace_back();
    }

    return newHandle;
}

void Dx12QueueManager::DestroyWork(GpuWorkHandle handle)
{
    PG_ASSERT(handle.isValid(mWork.size()));
    mWork[hande] = GpuWork();
    mFreeHandles.push_back(handle);
}

GpuWorkResultCode CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobCounts)
{
    return GpuWorkResultCode::Success;
}

}
}
