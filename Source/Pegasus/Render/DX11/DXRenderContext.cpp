/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderContext.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX implementation of render context

#if PEGASUS_GAPI_DX

#include <d3d11.h>
#include <dxgi1_2.h>
#include "Pegasus/Render/RenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXDevice.h"

#if PEGASUS_ENABLE_ASSERT
#define VALID_DECLARE(exp) HRESULT hr = exp;PG_ASSERT(hr==S_OK)
#define VALID(exp) hr = exp;PG_ASSERT(hr == S_OK)
#else
#define VALID_DECLARE(exp) exp
#define VALID(exp) exp
#endif

namespace Pegasus {
namespace Render {

Context::Context(const ContextConfig& config)
    : mAllocator(config.mAllocator),
      mParentDevice(config.mDevice)
{
    PG_ASSERT(mParentDevice != nullptr);
    mPrivateData = nullptr;
    DXDevice* parentDevice = static_cast<DXDevice*>(mParentDevice);

    IDXGIDevice2* dXGIDevice = NULL;
    VALID_DECLARE(parentDevice->GetDevice()->QueryInterface(__uuidof(IDXGIDevice2), (void**)&dXGIDevice));

    IDXGIAdapter*  dXGIAdapter = NULL;
    VALID(dXGIDevice->GetAdapter(&dXGIAdapter));

    IDXGIFactory2* dXGIFactory = NULL;
    VALID(dXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dXGIFactory));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;
    // Set the width and height of the back buffer.
    swapChainDesc.Width = 600;
    swapChainDesc.Height = 600;
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
    swapChainDesc.Flags = 0;

    IDXGISwapChain1* targetSwapChain;
    VALID(dXGIFactory->CreateSwapChainForHwnd(
        parentDevice->GetDevice(),
        (HWND)config.mOwnerWindowHandle,
        &swapChainDesc,
        NULL, //no fullscreen desc
        NULL,
        &targetSwapChain
    ));
}


Context::~Context()
{
}


void Context::Bind() const
{
}


void Context::Unbind() const
{ 
}


void Context::Swap() const
{
}

}//namespace Render
}//namespace Pegasus
#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
