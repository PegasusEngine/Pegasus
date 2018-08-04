/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12QueueManager.h
//! \author Kleber Garcia
//! \date   July 19th 2018
//! \brief  Implementation of command queue shenanigans

#pragma once

struct ID3D12CommandQueue;
struct ID3D12Device2;


namespace Pegasus
{
namespace Alloc
{
    class IAllocator;
}

namespace Render
{

class Dx12Device;
class Dx12Fence;

class Dx12QueueManager
{
public:
    Dx12QueueManager(Alloc::IAllocator* allocator, Dx12Device* device);
    ~Dx12QueueManager();
    
    ID3D12CommandQueue* GetDirect() { return mDirectQueue; }
    Dx12Fence* GetDirectFence() { return mDirectQueueFence; }

private:
    Alloc::IAllocator* mAllocator;
    ID3D12CommandQueue* mDirectQueue;
    Dx12Fence* mDirectQueueFence;
    ID3D12Device2* mDevice;
};

}
}
