#include "Dx12Display.h"
#include "Dx12Device.h"
#include "Dx12Defs.h"
#include "Dx12QueueManager.h"
#include <dxgi1_6.h>
#include <Pegasus/Allocator/IAllocator.h>

namespace Pegasus
{
namespace Render
{


Dx12Display::Dx12Display(const DisplayConfig& config, Alloc::IAllocator* alloc) 
: IDisplay(config, alloc), mSwapChain(nullptr)
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
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        
        // Discard the back buffer contents after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // Don't set the advanced flags.
        //swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.Flags = 0;


        DX_VALID_DECLARE(dXGIFactory->CreateSwapChainForHwnd(
            mDevice->GetQueueManager()->GetDirect(),
            (HWND)config.moduleHandle,
            &swapChainDesc,
            NULL, //no fullscreen desc
            NULL,
            &mSwapChain
        ));

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
