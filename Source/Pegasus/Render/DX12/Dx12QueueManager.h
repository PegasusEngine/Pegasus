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
#include "../Common/ResourceStateTable.h"
#include "../Common/JobTreeVisitors.h"
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
class Dx12Resource;
class CanonicalJobPath;

class Dx12QueueManager
{
public:
    enum class WorkType : unsigned
    {
        Graphics,
        Compute,
        Copy,
        Count
    };

    Dx12QueueManager(Pegasus::Alloc::IAllocator* allocator, Dx12Device* device);
    ~Dx12QueueManager();
    
    GpuWorkHandle AllocateWork(); 
    void DestroyWork(Pegasus::Render::GpuWorkHandle handle);
    GpuWorkResultCode CompileWork(GpuWorkHandle handle, const CanonicalJobPath* jobs, unsigned jobsCount);

    ID3D12CommandQueue* GetDirect() { return mQueueContainers[(int)WorkType::Graphics].queue; }

private:
    
    struct GpuList 
    {
        UINT64 fenceVal = 0ull;
        WorkType type = WorkType::Graphics;
        CComPtr<ID3D12GraphicsCommandList> list = nullptr;
        CComPtr<ID3D12CommandAllocator> allocator = nullptr;
    };

    using Dx12GpuLists = std::vector<GpuList>;
    using Dx12CmdLists = std::vector<CComPtr<ID3D12GraphicsCommandList>>;
    using Dx12CmdAllocators = std::vector<CComPtr<ID3D12CommandAllocator>>;

    struct GpuWork
    {
        std::vector<GpuList> gpuLists;
    };

    struct QueueContainer
    {
        Dx12Fence* fence = nullptr;
        ID3D12CommandQueue* queue = nullptr;
        Dx12CmdLists freeLists;
        Dx12CmdAllocators freeAllocators;
    };

    void AllocateList(WorkType workType, GpuWork& work);
    void TranspileList(const CanonicalJobPath& job, GpuList& gpuList);
    void GetD3DBarrier(const CanonicalJobPath::GpuBarrier& b, D3D12_RESOURCE_BARRIER& dx12Barrier) const;
    D3D12_RESOURCE_STATES GetD3D12State(const LocationGpuState& state, unsigned stateId, const Dx12Resource* dx12Resource) const;

    std::vector<GpuWork> mWorks;
    std::vector<GpuWorkHandle> mFreeHandles;

    QueueContainer mQueueContainers[(int)WorkType::Count];

    Alloc::IAllocator* mAllocator;
    Dx12Device* mDevice;

    ResourceStateTable::Domain mGlobalResourceStateDomain;
    ResourceStateTable* mGlobalResourceStateTable;
    std::vector<ResourceGpuState> mGlobalGpuStates;
};

}
}
