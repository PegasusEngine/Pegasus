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

namespace Pegasus
{

namespace Render
{

class Dx12Fence;
class Dx12Device;

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

private:
    struct FrameState
    {
        UINT64 fenceVal = 0;
        CComPtr<ID3D12CommandAllocator> cmdListAllocator;
    }* mFrameStates;

	FrameState& CurrFrame() { return mFrameStates[mFrameId % mMaxFrames]; }

    CComPtr<ID3D12GraphicsCommandList> mCmdList;

    Dx12Device* mDevice;
    Dx12Fence* mFence;
    ID3D12CommandQueue* mQueue;
    UINT mMaxFrames;
    UINT mFrameId;
};


}
}
