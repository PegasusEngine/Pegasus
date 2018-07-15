#include "Dx12Display.h"
#include "Dx12Device.h"
#include <Pegasus/Allocator/IAllocator.h>

#if PEGASUS_ENABLE_ASSERT
#define VALID_DECLARE(exp) HRESULT hr = exp;PG_ASSERT(hr==S_OK)
#define VALID(exp) hr = exp;PG_ASSERT(hr == S_OK)
#else
#define VALID_DECLARE(exp) exp
#define VALID(exp) exp
#endif

namespace Pegasus
{
namespace Render
{


Dx12Display::Dx12Display(const DisplayConfig& config, Alloc::IAllocator* alloc) 
: IDisplay(config, alloc)
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
        swapChainDesc.BufferCount = 1;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        
        // Discard the back buffer contents after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        // Don't set the advanced flags.
        //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.Flags = 0;

        //VALID(dXGIFactory->CreateSwapChainForHwnd(
        //    mDevice->GetD3D(),
        //    (HWND)config.mOwnerWindowHandle,
        //    &swapChainDesc,
        //    NULL, //no fullscreen desc
        //    NULL,
        //    &mSwapChain
        //));
    }
    
}

Dx12Display::~Dx12Display()
{
    if (mSwapChain)
    {
        mSwapChain->Release();
    }
}

void Dx12Display::BeginFrame()
{
}

void Dx12Display::EndFrame()
{
}

void Dx12Display::Resize(int width, int height)
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