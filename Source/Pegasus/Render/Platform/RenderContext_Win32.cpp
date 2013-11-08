/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext_Win32.cpp
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Platform-specific render context for Windows.

#if PEGASUS_PLATFORM_WINDOWS
#include "../Source/Pegasus/Render/Platform/RenderContext_Win32.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus {
namespace Render {

// Global pixel format descriptor for RGBA 32-bits
static PIXELFORMATDESCRIPTOR sPixelFormat = {
    sizeof(PIXELFORMATDESCRIPTOR), //! size of structure
    1, //! default version
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //! flags
    PFD_TYPE_RGBA, //! RGBA color mode
    32, //! 32 bit color mode
    0, 0, 0, 0, 0, 0, //! ignore color bits
    0, //! no alpha buffer
    0, //! ignore shift bit
    0, //! no accumulation buffer
    0, 0, 0, 0, //! ignore accumulation bits
    24, //! 24 bit z-buffer size
    8, //! 8 bit stencil-buffer size
    0, //! no aux buffer
    PFD_MAIN_PLANE, //! main drawing plane
    0, //! reserved
    0, 0, 0}; //! layer masks ignored


//----------------------------------------------------------------------------------------

IRenderContextImpl* IRenderContextImpl::CreateImpl(const ContextConfig& config, Alloc::IAllocator* alloc)
{
    return PG_NEW(alloc, -1, "RenderContext platform impl", Pegasus::Alloc::PG_MEM_PERM) RenderContextImpl_Win32(config);
}

//----------------------------------------------------------------------------------------

void IRenderContextImpl::DestroyImpl(IRenderContextImpl* impl, Alloc::IAllocator* alloc)
{
    PG_DELETE(alloc, impl);
}

//----------------------------------------------------------------------------------------

RenderContextImpl_Win32::RenderContextImpl_Win32(const ContextConfig& config)
    : mDeviceContextHandle((HDC) config.mDeviceContextHandle)
{
    // Create context
    if (config.mStartupContext)
    {
        // Startup
        int nPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat(mDeviceContextHandle, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mRenderContextHandle = wglCreateContext(mDeviceContextHandle);

        PG_LOG('OGL_', "Startup context created");
    }
    else
    {
        // Context attributes for OGL 4.3
        const int sAttrib[8] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                                WGL_CONTEXT_MINOR_VERSION_ARB , 3,
                                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                0, 0};

        // Full context
        int nPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat(mDeviceContextHandle, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mRenderContextHandle = wglCreateContextAttribsARB(mDeviceContextHandle, 0, sAttrib);
    }

    PG_LOG('OGL_', "Context %u created", mRenderContextHandle);

    // Link it to the window
    Bind();
}

//----------------------------------------------------------------------------------------

RenderContextImpl_Win32::~RenderContextImpl_Win32()
{
    // Unbind and destroy the context
    Unbind();
    wglDeleteContext(mRenderContextHandle);

    PG_LOG('OGL_', "Context %u destroyed", mRenderContextHandle);
}

//----------------------------------------------------------------------------------------

void RenderContextImpl_Win32::Bind() const
{
    wglMakeCurrent(mDeviceContextHandle, mRenderContextHandle);

    PG_LOG('OGL_', "%u is now the active context", mRenderContextHandle);
}

//----------------------------------------------------------------------------------------

void RenderContextImpl_Win32::Unbind() const
{
    wglMakeCurrent(mDeviceContextHandle, NULL);

    PG_LOG('OGL_', "NULL is now the active context");
}

//----------------------------------------------------------------------------------------

void RenderContextImpl_Win32::Swap() const
{
    // Present (no need for glFlush() since SwapBuffers() takes care of it)
    SwapBuffers(mDeviceContextHandle);
}


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
