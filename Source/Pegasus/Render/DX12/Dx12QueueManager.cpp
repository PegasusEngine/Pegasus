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
#include "Dx12Fence.h"

#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{

Dx12QueueManager::Dx12QueueManager(Alloc::IAllocator* allocator, Dx12Device* device)
: mDevice(device->GetD3D()), mAllocator(allocator)
{
    mDevice->AddRef();

    {
        D3D12_COMMAND_QUEUE_DESC qDesc = {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            D3D12_COMMAND_QUEUE_FLAG_NONE,
            0 /*node mask*/
        };

        DX_VALID_DECLARE(mDevice->CreateCommandQueue(&qDesc, __uuidof(mDirectQueue), &((void*)mDirectQueue)));
        mDirectQueueFence = D12_NEW(mAllocator, "Dx12Fence") Dx12Fence(device, mDirectQueue);
    }
}

Dx12QueueManager::~Dx12QueueManager()
{
    mDirectQueue->Release();
    mDevice->Release();
    D12_DELETE(mAllocator, mDirectQueueFence);
}

}
}
