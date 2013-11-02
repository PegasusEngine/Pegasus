/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContext.cpp
//! \author David Worsham
//! \date   5th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#include "Pegasus/Render/RenderContext.h"
#include <windows.h>
#define GLEW_STATIC 1
#include "Pegasus/Libs/GLEW/glew.h"
#include "Pegasus/Libs/GLEW/wglew.h"

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

ContextConfig::ContextConfig()
    : mDeviceContextHandle(0), mStartupContext(false)
{
}

//----------------------------------------------------------------------------------------

ContextConfig::~ContextConfig()
{
}

//----------------------------------------------------------------------------------------

Context::Context(const ContextConfig& config)
    : mDeviceContextHandle(config.mDeviceContextHandle)
{
    // Create context
    if (config.mStartupContext)
    {
        // Startup
        int nPixelFormat = ChoosePixelFormat((HDC) mDeviceContextHandle, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat((HDC) mDeviceContextHandle, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mRenderContextHandle = (RenderContextHandle) wglCreateContext((HDC) mDeviceContextHandle);
    }
    else
    {
        // Context attributes for OGL 4.3
        const int sAttrib[8] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                                WGL_CONTEXT_MINOR_VERSION_ARB , 3,
                                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                0, 0};

        //! \todo Assert on startup context created
        // Full context
        int nPixelFormat = ChoosePixelFormat((HDC) mDeviceContextHandle, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat((HDC) mDeviceContextHandle, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mRenderContextHandle = (RenderContextHandle) wglCreateContextAttribsARB((HDC) mDeviceContextHandle, 0, sAttrib);
    }

    // Link it to the window
    Bind();
}

//----------------------------------------------------------------------------------------

Context::~Context()
{
    // Unbind and destroy the context
    Unbind();
    wglDeleteContext((HGLRC) mRenderContextHandle);
}

//----------------------------------------------------------------------------------------

void Context::Bind() const
{
    wglMakeCurrent((HDC) mDeviceContextHandle, (HGLRC) mRenderContextHandle);
}

//----------------------------------------------------------------------------------------

void Context::Unbind() const
{
    wglMakeCurrent((HDC) mDeviceContextHandle, NULL);
}

//----------------------------------------------------------------------------------------

void Context::Swap() const
{
    // Present (no need for glFlush() since SwapBuffers() takes care of it)
    SwapBuffers((HDC) mDeviceContextHandle);
}


}   // namespace Render
}   // namespace Pegasus
