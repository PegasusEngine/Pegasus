/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderContext.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX implementation of render context

#if PEGASUS_GAPI_DX11



#include "Pegasus/Render/RenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXDevice.h"

namespace RenderPrivate
{
    Pegasus::Render::DXRenderContext * gBindedContext = nullptr;
};

namespace Pegasus {
namespace Render {

void GetDeviceAndContext(ID3D11Device ** device, ID3D11DeviceContext ** contextPointer)
{
    Pegasus::Render::DXRenderContext * context = Pegasus::Render::DXRenderContext::GetBindedContext();
    PG_ASSERT(context != nullptr);
    *contextPointer = context->GetD3D();
    PG_ASSERT(*contextPointer != nullptr);
    *device = context->GetDevice()->GetD3D();
    PG_ASSERT(*device != nullptr);
}


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
#if PEGASUS_GPU_DEBUG
  mUserDefinedAnnotation(nullptr),
#endif
  mSwapChain(nullptr),
  mFrameBuffer(nullptr),
  mFrameBufferDepthStencil(nullptr),
  mFrameBufferHeight(0),
  mFrameBufferWidth(0)
{

}

DXRenderContext::~DXRenderContext()
{
    if (mSwapChain != nullptr)
    {
        mSwapChain->Release();
    }
    if (mFrameBuffer != nullptr)
    {
        mFrameBuffer->Release();
    }
    if (mFrameBufferDepthStencil != nullptr)
    {
        mFrameBufferDepthStencil->Release();
    }
#if PEGASUS_GPU_DEBUG
    if (mUserDefinedAnnotation != nullptr)
    {
        mUserDefinedAnnotation->Release();
    }
#endif
}

void DXRenderContext::Present()
{   
    mSwapChain->Present(0,0);
}

bool DXRenderContext::Initialize(const ContextConfig& config)
{
    mDevice = static_cast<DXDevice*>(config.mDevice);
    mCachedD3DContext = mDevice->GetD3DImmContext();
   
    if (config.mOwnerWindowHandle != NULL)
    {
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
        swapChainDesc.Width = config.mWidth;
        swapChainDesc.Height = config.mHeight;

        mFrameBufferWidth = config.mWidth;
        mFrameBufferHeight = config.mHeight;

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

#if PEGASUS_GPU_DEBUG
       VALID(mCachedD3DContext->QueryInterface( __uuidof(mUserDefinedAnnotation), reinterpret_cast<void**>(&mUserDefinedAnnotation)));
#endif

        return mSwapChain != nullptr;
    }
    else
    {
        //This is a swap chain-less context. Use it for headless render.
        return true;
    }
}

void DXRenderContext::InitializeFrame()
{
    ID3D11Device * device = GetDevice()->GetD3D();
    ID3D11Texture2D* backBuffer = nullptr;;
    VALID_DECLARE(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer));
    VALID(device->CreateRenderTargetView(backBuffer, NULL, &mFrameBuffer));

    ID3D11Texture2D* depthStencilTexture = nullptr;
    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = mFrameBufferWidth;
    descDepth.Height = mFrameBufferHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    VALID(device->CreateTexture2D( &descDepth, NULL, &depthStencilTexture ));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    VALID(device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &mFrameBufferDepthStencil));

    backBuffer->Release();
    depthStencilTexture->Release();
}

#if PEGASUS_GPU_DEBUG
void DXRenderContext::BeginMarker(const char* marker)
{
    const int convertedSz = 256;
    wchar_t convertedDest[convertedSz];
    const wchar_t* targetMarker = L"<FAIL_STR>";
    int retVal = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, marker, -1, convertedDest, convertedSz);
    if (retVal > 0)
    {
        targetMarker = convertedDest;
    }

    if (mUserDefinedAnnotation != nullptr)
    {
        mUserDefinedAnnotation->BeginEvent(targetMarker);
    }
}

void DXRenderContext::EndMarker()
{
    if (mUserDefinedAnnotation != nullptr)
    {
        mUserDefinedAnnotation->EndEvent();
    }
}
#endif

void DXRenderContext::Resize(int width, int height)
{
    GetD3D()->OMSetRenderTargets(0,0,0);
	mFrameBuffer->Release();
    mFrameBuffer = nullptr;

    mFrameBufferDepthStencil->Release();
    mFrameBufferDepthStencil = nullptr;

    VALID_DECLARE(
        mSwapChain->ResizeBuffers(
            0, //buffers
            width,
            height,
            DXGI_FORMAT_UNKNOWN,
            0
        )
    );

    mFrameBufferWidth = width;
    mFrameBufferHeight = height;

    InitializeFrame();
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
    DXRenderContext * context = static_cast<DXRenderContext*>(mPrivateData);
    if (context == RenderPrivate::gBindedContext)
        Unbind();
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

void Context::Resize(int width, int height)
{
    DXRenderContext * context = static_cast<DXRenderContext*>(mPrivateData);
    context->Resize(width, height);
}

}//namespace Render
}//namespace Pegasus
#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
