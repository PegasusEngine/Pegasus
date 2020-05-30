#include "Dx12Display.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include "Dx12Fence.h"
#include "Dx12GpuProgram.h"
#include "Dx12Pso.h"
#include "Dx12Resources.h"
#include "Dx12Fence.h"
#include <dxgi1_6.h>
#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{


Dx12Display::Dx12Display(const DisplayConfig& config, Dx12Device* parentDevice) 
: IDisplay(config, parentDevice->GetAllocator())
, mSwapChain(nullptr)
, mDevice(parentDevice)
{
    {          
		IDXGIFactory4* dXGIFactory = mDevice->GetGraphicsCardInfos()->dxgiFactory;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        // Set to multiple back buffers.
        swapChainDesc.BufferCount = 2;

        // Set the width and height of the back buffer.
        swapChainDesc.Width = mConfig.width;
        swapChainDesc.Height = mConfig.height;

        // Set regular 32-bit surface for the back buffer.
        swapChainDesc.Format = GetDxFormat(mConfig.format);
        swapChainDesc.Stereo = FALSE;

        // Turn multisampling off.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = mConfig.buffering;
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
            (HWND)config.windowHandle,
            &swapChainDesc,
            NULL, //no fullscreen desc
            NULL,
            &swapChain1
        ));

		DX_VALID(swapChain1->QueryInterface(&mSwapChain));

        TextureConfig surfaceConfig; 
        surfaceConfig.type = TextureType_2d;
        surfaceConfig.width = mConfig.width;
        surfaceConfig.height = mConfig.height;
        surfaceConfig.depth = 1;
        surfaceConfig.mipLevels = 1;
        surfaceConfig.name = "DisplayBuffer";
        surfaceConfig.format = mConfig.format;
        surfaceConfig.bindFlags = BindFlags_Rt;
        surfaceConfig.usage = ResourceUsage_Static;

        for (int i = 0; i < (int)mConfig.buffering; ++i)
        {
            ID3D12Resource* resource = nullptr;
            //Record RTVs
            DX_VALID(mSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resource)));

            Dx12TextureRef t = D12_NEW(parentDevice->GetAllocator(), "DisplayBufferAlloc") Dx12Texture(surfaceConfig, parentDevice);
            t->AcquireD3D12Resource(resource);
            t->init();
            mTextures.push_back(t);
    
            RenderTargetConfig rtConfig;
            rtConfig.colorCount = 1u;
            rtConfig.colors[0] = &(*t);
            Dx12RenderTargetRef rt = D12_NEW(parentDevice->GetAllocator(), "DisplayBufferRt") Dx12RenderTarget(rtConfig, parentDevice);
            mRts.push_back(rt);

            mFenceVals.push_back(0ull);
        }
    }
}

Dx12Display::~Dx12Display()
{
    if (mSwapChain)
    {
        mSwapChain->Release();
    }	
}

Texture* Dx12Display::GetTexture()
{
    return &(*(mTextures[mSwapChain->GetCurrentBackBufferIndex()]));
}

RenderTarget* Dx12Display::GetRenderTarget()
{
	auto rtId = mSwapChain->GetCurrentBackBufferIndex();
    return &(*(mRts[rtId]));
}

UINT64 Dx12Display::GetFenceVal() const
{
    return mFenceVals[mSwapChain->GetCurrentBackBufferIndex()];
}

void Dx12Display::Present(Dx12Fence* fence)
{
    auto bufferIndex = mSwapChain->GetCurrentBackBufferIndex();
    DX_VALID_DECLARE(mSwapChain->Present(1u, 0u));
    mFenceVals[bufferIndex] = fence->Signal();
}

void Dx12Display::Resize(unsigned int width, unsigned int height)
{
    if (width != mConfig.width || height != mConfig.height)
    {
    }
}

}
}
