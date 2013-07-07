/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PegasusRenderContext.cpp
//! \author David Worsham
//! \date   5th July 2013
//! \brief  Class that encapsulates an OGL rendering context.

#include "Pegasus/Render/PegasusRenderContext.h"
#include <windows.h>
#include "Pegasus/Libs/GLEW/glew.h"
#include "Pegasus/Libs/GLEW/wglew.h"


namespace Pegasus {
namespace Render {

// Global pixel format descriptor for RGBA 32-bits
//! \todo Support more pixel format types
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


//! config-based contructor.
//! \param config Configuration structure used to make this context.
Context::Context(const ContextConfig& config)
    : mHWND(config.mHWND)
{
    //! \todo Assert on trying to create startup twice
    // Create context
    if (config.mStartupContext)
    {
        // Startup
        HDC hdc = GetDC((HWND) mHWND);
        int nPixelFormat = ChoosePixelFormat(hdc, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat(hdc, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mHGLRC = (PG_HGLRC) wglCreateContext(hdc);
    }
    else
    {
        // Context attribtes for OGL 4.3
        const int sAttrib[6] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                                WGL_CONTEXT_MINOR_VERSION_ARB , 3,
                                0, 0};

        //! \todo Assert on startup context created
        // Full context
        HDC hdc = GetDC((HWND) mHWND);
        int nPixelFormat = ChoosePixelFormat(hdc, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat(hdc, nPixelFormat, &sPixelFormat);

        // Make a new opengl context
        mHGLRC = (PG_HGLRC) wglCreateContextAttribsARB(hdc, 0, sAttrib);
    }

    // Link it to the window
    Bind();
}

//----------------------------------------------------------------------------------------

//! Basic destructor.
Context::~Context()
{
    // Unbind and destroy the context
    Unbind();
    wglDeleteContext((HGLRC) mHGLRC);
}

//----------------------------------------------------------------------------------------

//! Binds this context to the calling thread and makes it active.
void Context::Bind() const
{
    HDC hdc = GetDC((HWND) mHWND);

    wglMakeCurrent(hdc, (HGLRC) mHGLRC);
}

//----------------------------------------------------------------------------------------

//! Unbinds this context from the current thread and deactivates it.
void Context::Unbind() const
{
    HDC hdc = GetDC((HWND) mHWND);

    wglMakeCurrent(hdc, NULL);
}

//----------------------------------------------------------------------------------------

//! Performs a swap of the system backbuffer chain.
void Context::Swap() const
{
    HDC hdc = GetDC((HWND) mHWND);

    // Present
    glFlush();
    SwapBuffers(hdc);
}

//----------------------------------------------------------------------------------------

}   // namespace Render
}   // namespace Pegasus
