#include "Dx12Display.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include "Dx12Fence.h"
#include "Dx12GpuProgram.h"
#include "Dx12Pso.h"
#include <dxgi1_6.h>
#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{


Dx12Display::Dx12Display(const DisplayConfig& config, Alloc::IAllocator* alloc) 
: IDisplay(config, alloc), mSwapChain(nullptr), mBackBufferIdx(0u)
{
    PG_ASSERT(config.device);
    PG_ASSERT(config.moduleHandle != NULL);

    mDevice = static_cast<Dx12Device*>(config.device);
    mWidth = config.width;
    mHeight = config.height;
    
    {          
		IDXGIFactory4* dXGIFactory = mDevice->GetGraphicsCardInfos()->dxgiFactory;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        // Set to multiple back buffers.
        swapChainDesc.BufferCount = 2;

        // Set the width and height of the back buffer.
        swapChainDesc.Width = mWidth;
        swapChainDesc.Height = mHeight;

        // Set regular 32-bit surface for the back buffer.
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;

        // Turn multisampling off.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = GetBuffering();
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        
        // Discard the back buffer contents after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // Don't set the advanced flags.
        //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.Flags = 0;

		CComPtr<IDXGISwapChain1> swapChain1;

        DX_VALID_DECLARE(dXGIFactory->CreateSwapChainForHwnd(
            mDevice->GetQueueManager()->GetDirect(),
            (HWND)config.moduleHandle,
            &swapChainDesc,
            NULL, //no fullscreen desc
            NULL,
            &swapChain1
        ));

		DX_VALID(swapChain1->QueryInterface(&mSwapChain));

        for (int i = 0; i < (int)GetBuffering(); ++i)
        {
            //Record RTVs
            DX_VALID(mSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&mColorResources[i])));
            
            mRtvBuffers[i] = mDevice->GetMemMgr()->AllocateRenderTarget();
            mDevice->GetD3D()->CreateRenderTargetView(mColorResources[i], nullptr, mRtvBuffers[i].handle);
            mFenceValues[i] = 0u;

            //Create command list allocator
            DX_VALID(
                mDevice->GetD3D()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&mCmdListsAllocator[i])));

        }

        mBackBufferIdx = mSwapChain->GetCurrentBackBufferIndex();

        //Create command list
        DX_VALID(
            mDevice->GetD3D()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            mCmdListsAllocator[mBackBufferIdx],
            nullptr, //Null PSO
            __uuidof(ID3D12GraphicsCommandList),
            reinterpret_cast<void**>(&mCmdList))
        );

		DX_VALID(mCmdList->Close());
    }
}

void Dx12Display::Flush()
{
    Dx12QueueManager* qManager = mDevice->GetQueueManager();
    Dx12Fence* directQueueFence = qManager->GetDirectFence();
    for (int i = 0; i < (int)GetBuffering(); ++i)
    {
        directQueueFence->WaitOnCpu(mFenceValues[i]);
    }
}

Dx12Display::~Dx12Display()
{
    Flush();

    for (int i = 0; i < (int)GetBuffering(); ++i)
    {
        mDevice->GetMemMgr()->Delete(mRtvBuffers[i]);
    } 

    if (mSwapChain)
    {
        mSwapChain->Release();
    }
}

void Dx12Display::BeginFrame()
{
    Dx12QueueManager* qManager = mDevice->GetQueueManager();
    ID3D12CommandQueue* directQueue = qManager->GetDirect();
    Dx12Fence* directQueueFence = qManager->GetDirectFence();
    
    //update to next frame
    mBackBufferIdx = mSwapChain->GetCurrentBackBufferIndex();
    
    //Wait till this back buffer has finished its fence
    directQueueFence->WaitOnCpu(mFenceValues[mBackBufferIdx]);

    //We are done! now we can proceed to render! lets acquire the current draw list
    DX_VALID_DECLARE(mCmdListsAllocator[mBackBufferIdx]->Reset());
    DX_VALID(mCmdList->Reset(mCmdListsAllocator[mBackBufferIdx], nullptr));

    D3D12_RESOURCE_BARRIER barrierDesc;
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.pResource = mColorResources[mBackBufferIdx];
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    mCmdList->ResourceBarrier(1, &barrierDesc);

    mCmdList->OMSetRenderTargets(1, &mRtvBuffers[mBackBufferIdx].handle, false, nullptr);
    FLOAT clearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    mCmdList->ClearRenderTargetView(mRtvBuffers[mBackBufferIdx].handle, clearColor, 0, nullptr);
    
}

void Dx12Display::EndFrame()
{
    Dx12QueueManager* qManager = mDevice->GetQueueManager();
    ID3D12CommandQueue* directQueue = qManager->GetDirect();
    Dx12Fence* directQueueFence = qManager->GetDirectFence();

    D3D12_RESOURCE_BARRIER barrierDesc;
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.pResource = mColorResources[mBackBufferIdx];
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    mCmdList->ResourceBarrier(1, &barrierDesc);
	DX_VALID_DECLARE(mCmdList->Close());

	ID3D12CommandList* lists[] = { mCmdList };
    directQueue->ExecuteCommandLists(1, lists);
    DX_VALID(mSwapChain->Present(1u, 0u));

    mFenceValues[mBackBufferIdx] = directQueueFence->Signal();

}

void Dx12Display::Resize(unsigned int width, unsigned int height)
{
    if (width != mWidth || height != mHeight)
    {
    }
}

IDisplay* IDisplay::CreatePlatformDisplay(const DisplayConfig& displayConfig, Alloc::IAllocator* alloc)
{
    return PG_NEW(alloc, -1, "Dx12Display", Pegasus::Alloc::PG_MEM_PERM) Dx12Display(displayConfig, alloc);
}

}
}
