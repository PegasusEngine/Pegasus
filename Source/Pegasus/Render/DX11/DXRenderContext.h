/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderContext.h
//! \author Platform wrapper for dx11 context
//! \date   18th July 2013
//! \brief  Class that encapsulates a render library rendering context and swap chain

#ifndef PEGASUS_DX_CONTEXT_H
#define PEGASUS_DX_CONTEXT_H

#include "Pegasus/Render/RenderContextConfig.h"
#include <d3d11.h>
#include <dxgi1_2.h>

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

//! convinience function to get the d3d11 device and contexts
void GetDeviceAndContext(ID3D11Device ** device, ID3D11DeviceContext ** contextPointer);

class DXDevice;

//! Class that wraps the active context in the application
class DXRenderContext
{
public:

    //! constructor
    DXRenderContext();

    //! destructor
    ~DXRenderContext(); 

    //! initializer 
    //! \param device the d3d valid device
    //! \return true if successful, false if the swap chain initialization failed
    bool Initialize(const ContextConfig& config);

    //! \return the binded context
    static DXRenderContext* GetBindedContext();

    //! bind the current context passed for global access
    static void BindRenderContext(DXRenderContext* context);

    //! final function that presents the frame and swaps
    void Present();

    //! callback when the window has resized
    void Resize(int width, int height);

    //! get the device
    DXDevice* GetDevice() const { return mDevice; }

    //! get the device context
    ID3D11DeviceContext * GetD3D() const { return mCachedD3DContext; }

    //! get the render target
    ID3D11RenderTargetView* GetRenderTarget() { return mFrameBuffer; }

    //! get the depth stencil
    ID3D11DepthStencilView* GetDepthStencil() { return mFrameBufferDepthStencil; }

private:

    //! initializes a frame for rendering
    void InitializeFrame();

    //! device wrapper
    DXDevice*            mDevice;

    //! d3d elements
    ID3D11DeviceContext* mCachedD3DContext;

    //! the swap chain
    IDXGISwapChain1*     mSwapChain;

    //! frame buffer
    ID3D11RenderTargetView* mFrameBuffer;

    //! depth stencil
    ID3D11DepthStencilView* mFrameBufferDepthStencil;

    //! target frame buffer width
    int mFrameBufferWidth;

    //! target frame buffer height
    int mFrameBufferHeight;

};

}
}

#endif
