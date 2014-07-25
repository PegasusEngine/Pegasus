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



#include "Pegasus/Render/RenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXDevice.h"

#if PEGASUS_ENABLE_ASSERT
#define VALID_DECLARE(exp) HRESULT hr = exp;PG_ASSERT(hr==S_OK)
#define VALID(exp) hr = exp;PG_ASSERT(hr == S_OK)
#else
#define VALID_DECLARE(exp) exp
#define VALID(exp) exp
#endif

namespace RenderPrivate
{
    Pegasus::Render::DXRenderContext * gBindedContext = nullptr;
};

namespace Pegasus {
namespace Render {


DXRenderContext* DXRenderContext::GetBindedContext()
{
    return RenderPrivate::gBindedContext;
}

void DXRenderContext::BindRenderContext(DXRenderContext* context)
{
    RenderPrivate::gBindedContext = context;
}


DXRenderContext::DXRenderContext()
 :
  mDevice(nullptr),
  mCachedD3DContext(nullptr),
  mSwapChain(nullptr),
  mFrameBuffer(nullptr)
{
}

DXRenderContext::~DXRenderContext()
{
    mSwapChain->Release();
    if (mFrameBuffer)
    {
        mFrameBuffer->Release();
    }
}

void DXRenderContext::Present()
{
    mSwapChain->Present(0,0);
}

bool DXRenderContext::Initialize(const ContextConfig& config)
{
    mDevice = static_cast<DXDevice*>(config.mDevice);
    mCachedD3DContext = mDevice->GetD3DImmContext();
   
    IDXGIDevice2* dXGIDevice = NULL;
    VALID_DECLARE(mDevice->GetD3D()->QueryInterface(__uuidof(IDXGIDevice2), (void**)&dXGIDevice));
  
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

    VALID(dXGIFactory->CreateSwapChainForHwnd(
        mDevice->GetD3D(),
        (HWND)config.mOwnerWindowHandle,
        &swapChainDesc,
        NULL, //no fullscreen desc
        NULL,
        &mSwapChain
    ));

    //bypass Com pointers by releasing these resources manually
    dXGIDevice->Release();
    dXGIAdapter->Release();
    dXGIFactory->Release();

    if (mSwapChain != NULL)
    {
        InitializeFrame();
    }

    return mSwapChain != nullptr;
    return true;
}

void DXRenderContext::InitializeFrame()
{
    ID3D11Texture2D* backBuffer;
    VALID_DECLARE(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer));
    GetDevice()->GetD3D()->CreateRenderTargetView(backBuffer, NULL, &mFrameBuffer);
    backBuffer->Release();
}

Context::Context(const ContextConfig& config)
    : mAllocator(config.mAllocator),
      mParentDevice(config.mDevice),
      mPrivateData(nullptr)
{
    PG_ASSERT(mParentDevice != nullptr);
    DXRenderContext * renderContext = PG_NEW(
        mAllocator,
        -1,
        "DXRenderContext",
        Alloc::PG_MEM_PERM
    ) DXRenderContext();

    if (renderContext->Initialize(config))
    {
        mPrivateData = static_cast<PrivateContextData>(renderContext);
    }
    else
    {
        PG_FAILSTR("failed creating swap chain / context");
    }

    Bind();
}


Context::~Context()
{
    PG_DELETE(mAllocator, static_cast<DXRenderContext*>(mPrivateData));
}


void Context::Bind() const
{
    DXRenderContext * context = static_cast<DXRenderContext*>(mPrivateData);
    DXRenderContext::BindRenderContext(context);
}


void Context::Unbind() const
{ 
    DXRenderContext::BindRenderContext(NULL);
}


void Context::Swap() const
{
    DXRenderContext * context = static_cast<DXRenderContext*>(mPrivateData);
    context->Present();
}

}//namespace Render
}//namespace Pegasus
#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
