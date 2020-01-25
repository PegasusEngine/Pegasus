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
struct ID3D12CommandList;
struct ID3D12CommandAllocator;

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
class CanonicalJobPath;

class Dx12QueueManager
{
public:
    enum class WorkType : unsigned
    {
        Compute,
        Graphics,
        Copy,
        WorkTypeCount
    };

    Dx12QueueManager(Alloc::IAllocator* allocator, Dx12Device* device);
    ~Dx12QueueManager();
    
    ID3D12CommandQueue* GetDirect() { return mDirectQueue; }
    GpuWorkHandle AllocateWork(); 
    void DestroyWork(GpuWorkHandle handle);
    GpuWorkResultCode CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobsCount);

private:
    using Dx12CmdLists = std::vector<CComPtr<ID3D12CommandList>>;
    using Dx12CmdAllocators = std::vector<CComPtr<ID3D12CommandAllocator>>;

    struct GpuWork
    {
        Dx12CmdLists lists;
    };

    struct QueueContainer
    {
        Dx12CmdLists freeLists;
        Dx12Fence* fence = nullptr;
        ID3D12CommandQueue* queue = nullptr;
    };

    std::vector<GpuWork> mWork;
    std::vector<GpuWorkHandle> mFreeHandles;

    QueueContainer mQueueContainers[(int)WorkTypeCount];
    Dx12CmdAllocators mCmdAllocators;

    Alloc::IAllocator* mAllocator;
    ID3D12CommandQueue* mDirectQueue;
    ID3D12Device2* mDevice;
};

}
}
