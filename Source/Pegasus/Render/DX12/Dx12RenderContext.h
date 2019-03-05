/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12RenderContext.h
//! \author Kleber Garcia
//! \date   March 4 2019
//! \brief  Implementation of render context

#pragma once

#include <D3D12.h>
#include <atlbase.h>
#include "Dx12RDMgr.h"
#include "Dx12Resources.h"
#include "Dx12Pso.h"

namespace Pegasus
{

namespace Render
{

class Dx12Fence;
class Dx12Device;
class Dx12FramePayload;
struct ResourceDescriptor;
struct Dx12DrawState;
class Dx12ResourceTable;
class Dx12Pso;

class Dx12RenderContext
{
public:
    Dx12RenderContext(Dx12Device* device, UINT maxFrames, ID3D12CommandQueue* queue);
    ~Dx12RenderContext();

    UINT GetMaxFrames() const { return mMaxFrames; }
    UINT GetFrameId() const { return mFrameId % mMaxFrames; }
    void BeginFrame();
    void EndFrame();
    void Flush();
    
	CComPtr<ID3D12GraphicsCommandList> GetCmdList() { return mCmdList;  }
    void SetResourceTable(Dx12ResourceTableRef table, UINT space);
    void SetPso(Dx12PsoRef pso);
    void Draw();

private:
    struct FrameState
    {
        UINT64 fenceVal = 0;
        CComPtr<ID3D12CommandAllocator> cmdListAllocator;
        Dx12FramePayload* payload;
        Dx12DrawState* state;
    }* mFrameStates;

    struct GpuRDTables
    {
        UINT totalSize = 0u;
        UINT sizeLeft = 0u;
        UINT offset = 0u;
        UINT incrSize = 0u;
        CComPtr<ID3D12DescriptorHeap> heap;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuDescStart;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuDescStart;
    } mRDTableHeaps[Dx12RDMgr::TableTypeMax];

	FrameState& CurrFrame() { return mFrameStates[mFrameId % mMaxFrames]; }

    void flushDrawState();
    bool allocateGpuTable(const Dx12RDMgr::Table& table, ResourceDescriptor& rd);

    CComPtr<ID3D12GraphicsCommandList> mCmdList;

    Dx12Device* mDevice;
    Dx12Fence* mFence;
    ID3D12CommandQueue* mQueue;
    UINT mMaxFrames;
    UINT mFrameId;
};


}
}
