#include "Dx12RenderContext.h"
#include "Dx12Fence.h"
#include "Dx12Defs.h"
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{


Dx12RenderContext::Dx12RenderContext(Dx12Device* device, UINT maxFrames, ID3D12CommandQueue* queue)
: mQueue(queue), mMaxFrames(maxFrames), mFrameId(0)
{
    mQueue->AddRef();
    mDevice = device;
    mFence = D12_NEW(device->GetAllocator(), "Dx12Fence") Dx12Fence(device, mQueue);
    mFrameStates = new FrameState[maxFrames];
    for (UINT f = 0; f < maxFrames; ++f)
    {
        auto& frameState = mFrameStates[f];
        DX_VALID_DECLARE(
            mDevice->GetD3D()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
           __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&frameState.cmdListAllocator)));
    }

	DX_VALID_DECLARE(
        mDevice->GetD3D()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        CurrFrame().cmdListAllocator,
        nullptr, //Null PSO
        __uuidof(ID3D12GraphicsCommandList),
        reinterpret_cast<void**>(&mCmdList))
    );

	DX_VALID(mCmdList->Close());
}

Dx12RenderContext::~Dx12RenderContext()
{
    Flush();
    D12_DELETE(mDevice->GetAllocator(), mFence);
    mQueue->Release();
    delete [] mFrameStates;
}

void Dx12RenderContext::Flush()
{
    for (UINT f = 0; f < mMaxFrames; ++f)
    {
        mFence->WaitOnCpu(mFrameStates[(mFrameId + f) % mMaxFrames].fenceVal);
    }
    mFrameId = 0;
}

void Dx12RenderContext::BeginFrame()
{
    auto& frame = CurrFrame();
    mFence->WaitOnCpu(frame.fenceVal);
    DX_VALID_DECLARE(frame.cmdListAllocator->Reset());
    DX_VALID(mCmdList->Reset(frame.cmdListAllocator, nullptr));
}

void Dx12RenderContext::EndFrame()
{
    auto& frame = CurrFrame();
    frame.fenceVal = mFence->Signal();
    ++mFrameId;
}

}
}
