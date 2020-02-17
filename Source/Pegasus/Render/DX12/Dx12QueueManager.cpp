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

static D3D12_COMMAND_LIST_TYPE GetCmdListType(Dx12QueueManager::WorkType workType)
{
    static D3D12_COMMAND_LIST_TYPE sCmdListTypes[(int)Dx12QueueManager::WorkType::Count] = {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_COPY
    };

    return sCmdListTypes[(unsigned)workType];
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
}

Dx12QueueManager::~Dx12QueueManager()
{
    for (int queueIt = 0; queueIt < (int)WorkType::Count; ++queueIt)
        mQueueContainers[queueIt].queue->Release();
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
        newHandle = mWork.size();
        mWork.emplace_back();
    }

    return newHandle;
}

void Dx12QueueManager::DestroyWork(GpuWorkHandle handle)
{
    PG_ASSERT(handle.isValid(mWork.size()));
    mWork[handle] = GpuWork();
    mFreeHandles.push_back(handle);
}

GpuWorkResultCode CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobCounts)
{
    return GpuWorkResultCode::Success;
}

}
}
