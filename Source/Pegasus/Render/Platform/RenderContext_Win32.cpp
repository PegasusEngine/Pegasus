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
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "../Source/Pegasus/Render/GL/GLExtensions.h"

namespace Pegasus {
namespace Render {

static HGLRC gRenderContextHandle = 0; //!< Opaque GL context handle
static int   gRenderContextHandleReferences = 0; //reference count for the GL context

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
    if (config.mStartupContext && gRenderContextHandle == 0)
    {
        // Startup
        int nPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &sPixelFormat);

        // Setup pixel format for backbuffer
        SetPixelFormat(mDeviceContextHandle, nPixelFormat, &sPixelFormat);

        if (gRenderContextHandleReferences == 0)
        {
            // Make a new OpenGL context
            gRenderContextHandle = wglCreateContext(mDeviceContextHandle);
        }

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
        if (gRenderContextHandleReferences == 0)
        {
            // Make a new OpenGL context
            gRenderContextHandle = wglCreateContextAttribsARB(mDeviceContextHandle, 0, sAttrib);
        }
   
        PG_ASSERTSTR(gRenderContextHandle != 0x0, "Cannot instantiate render context! Check if your graphcis card supports the minor and major version");
    }

    ++gRenderContextHandleReferences;

    PG_LOG('OGL_', "Context %u created", gRenderContextHandle);

    // Link it to the window
    Bind();
}

//----------------------------------------------------------------------------------------

RenderContextImpl_Win32::~RenderContextImpl_Win32()
{
    // Unbind and destroy the context
    Unbind();
    --gRenderContextHandleReferences;

    if (gRenderContextHandleReferences == 0)
    {
        wglDeleteContext(gRenderContextHandle);
        gRenderContextHandle = 0;
    }

    PG_LOG('OGL_', "Context %u destroyed", gRenderContextHandle);
}

//----------------------------------------------------------------------------------------
void RenderContextImpl_Win32::Bind() const
{    
    wglMakeCurrent(mDeviceContextHandle, gRenderContextHandle);

    PG_LOG('OGL_', "%u is now the active context", gRenderContextHandle);
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

//----------------------------------------------------------------------------------------
void RenderContextImpl_Win32::CheckRenderingExtensions() const
{
    // do a full check on rendering extensions supported by OpenGL

    // Write some temporary debugging information
    Render::GLExtensions extensions;
    switch (extensions.GetMaximumProfile())
    {
    case Render::GLExtensions::PROFILE_GL_3_3:
        PG_LOG('OGL_', "OpenGL 3.3 is the maximum detected profile.");
        break;

    case Render::GLExtensions::PROFILE_GL_4_3:
        PG_LOG('OGL_', "OpenGL 4.3 is the maximum detected profile.");
        break;

    default:
        PG_LOG('OGL_', "Error when initializing GLextensions->");
        break;
    }
    if (extensions.IsGLExtensionSupported("GL_ARB_draw_indirect"))
    {
        PG_LOG('OGL_', "GL_ARB_draw_indirect detected.");
    }
    else
    {
        PG_LOG('OGL_', "GL_ARB_draw_indirect NOT detected.");
    }
    if (extensions.IsGLExtensionSupported("GL_ATI_fragment_shader"))
    {
        PG_LOG('OGL_', "GL_ATI_fragment_shader detected.");
    }
    else
    {
        PG_LOG('OGL_', "GL_ATI_fragment_shader NOT detected.");
    }
    if (extensions.IsWGLExtensionSupported("WGL_ARB_buffer_region"))
    {
        PG_LOG('OGL_', "WGL_ARB_buffer_region detected.");
    }
    else
    {
        PG_LOG('OGL_', "WGL_ARB_buffer_region NOT detected.");
    }
    if (extensions.IsWGLExtensionSupported("WGL_3DL_stereo_control"))
    {
        PG_LOG('OGL_', "WGL_3DL_stereo_control detected.");
    }
    else
    {
        PG_LOG('OGL_', "WGL_3DL_stereo_control NOT detected.");
    }
}


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
