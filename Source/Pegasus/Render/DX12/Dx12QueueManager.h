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
#include <Pegasus/Render/IDevice.h>
#include <d3d12.h>
#include <atlbase.h>
#include <vector>

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
        Count
    };

    Dx12QueueManager(Alloc::IAllocator* allocator, Dx12Device* device);
    ~Dx12QueueManager();
    
    GpuWorkHandle AllocateWork(); 
    void DestroyWork(Pegasus::Render::GpuWorkHandle handle);
    GpuWorkResultCode CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobsCount);

    ID3D12CommandQueue* GetDirect() { return mQueueContainers[(int)WorkType::Graphics].queue; }

private:
    using Dx12CmdLists = std::vector<CComPtr<ID3D12GraphicsCommandList>>;
    using Dx12CmdAllocators = std::vector<CComPtr<ID3D12CommandAllocator>>;

    struct GpuWork
    {
        Dx12CmdLists lists;
        Dx12CmdAllocators activeAllocators;
    };

    struct QueueContainer
    {
        Dx12Fence* fence = nullptr;
        ID3D12CommandQueue* queue = nullptr;
    };

    std::vector<GpuWork> mWork;
    std::vector<GpuWorkHandle> mFreeHandles;

    QueueContainer mQueueContainers[(int)WorkType::Count];

    Dx12CmdLists mFreeLists;
    Dx12CmdAllocators mFreeAllocators;

    Alloc::IAllocator* mAllocator;
    Dx12Device* mDevice;
};

}
}
