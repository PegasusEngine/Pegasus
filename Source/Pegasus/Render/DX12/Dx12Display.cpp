#include "Dx12Display.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include "Dx12Fence.h"
#include "Dx12GpuProgram.h"
#include "Dx12Pso.h"
#include "Dx12RenderContext.h"
#include <dxgi1_6.h>
#include <Pegasus/Allocator/IAllocator.h>
#include <Pegasus/Core/Formats.h>

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
    mRenderContext = D12_NEW(mDevice->GetAllocator(), "Dx12RenderContext") Dx12RenderContext(mDevice, Buffering, mDevice->GetQueueManager()->GetDirect());
    mCmdList = mRenderContext->GetCmdList();
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
            
            mRtvBuffers[i] = mDevice->GetRDMgr()->AllocateRenderTarget();
            mDevice->GetD3D()->CreateRenderTargetView(mColorResources[i], nullptr, mRtvBuffers[i].handle);

        }

        mBackBufferIdx = mSwapChain->GetCurrentBackBufferIndex();
    }
}

void Dx12Display::Flush()
{
    mRenderContext->Flush();
}

Dx12Display::~Dx12Display()
{
    Flush();

    for (int i = 0; i < (int)GetBuffering(); ++i)
    {
        mDevice->GetRDMgr()->Delete(mRtvBuffers[i]);
    } 

    if (mSwapChain)
    {
        mSwapChain->Release();
    }	
    
    D12_DELETE(mDevice->GetAllocator(), mRenderContext);
}

void Dx12Display::BeginFrame()
{
    //update to next frame
    mRenderContext->BeginFrame();

    Dx12QueueManager* qManager = mDevice->GetQueueManager();
    ID3D12CommandQueue* directQueue = qManager->GetDirect();
    

	mBackBufferIdx = mSwapChain->GetCurrentBackBufferIndex();
    PG_ASSERT(mBackBufferIdx == mRenderContext->GetFrameId());


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
    
#if 0
    if (mProgramTest == nullptr)
    {
        Dx12ProgramDesc desc;
        desc.filename = "Shaders/hlsl/Dx12Test.hlsl";
        desc.mainNames[Dx12_Vertex] = "vsMain";
        desc.mainNames[Dx12_Pixel] = "psMain";
        mProgramTest = mDevice->CreateGpuProgram();
		mProgramTest->Compile(desc);
    }
	
    if (mTestTexture == nullptr)
    {
        TextureDesc desc;
        desc.name = "TestTexture";
        desc.type = TextureType_2d;
        desc.width = 256;
        desc.height = 256;
        desc.depth = 1;
        desc.mipLevels = 1;
        desc.format = Core::FORMAT_RGBA_8_UNORM;
        desc.bindFlags = BindFlags_Srv | BindFlags_Rt | BindFlags_Uav;
        desc.usage = ResourceUsage_Static;
        mTestTexture = mDevice->CreateTexture(desc);
    }

    if (mTestBuffer == nullptr)
    {
        BufferDesc desc;
        desc.name = "TestBuffer";
        desc.stride = sizeof(float);
        desc.elementCount = 34;
		desc.bindFlags = BindFlags_Srv | BindFlags_Uav;
        desc.bufferType = BufferType_Default;
		desc.usage = ResourceUsage_Static;
		desc.format = Core::FORMAT_R32_FLOAT;
        mTestBuffer = mDevice->CreateBuffer(desc);
    }
#endif

}

void Dx12Display::EndFrame()
{
    Dx12QueueManager* qManager = mDevice->GetQueueManager();
    ID3D12CommandQueue* directQueue = qManager->GetDirect();

    D3D12_RESOURCE_BARRIER barrierDesc;
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.pResource = mColorResources[mBackBufferIdx];
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    mCmdList->ResourceBarrier(1, &barrierDesc);

	mCmdList->Close();

	ID3D12CommandList* lists[] = { mCmdList };
    directQueue->ExecuteCommandLists(1, lists);
    DX_VALID_DECLARE(mSwapChain->Present(1u, 0u));

	mRenderContext->EndFrame();



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
