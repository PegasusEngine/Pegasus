/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLDeviceWin32.cpp
//! \author Kleber Garcia
//! \date   7th July 2014
//! \brief  Implementation of a drawing device with opengl on win32

#if PEGASUS_PLATFORM_WINDOWS && PEGASUS_GAPI_GL

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../Source/Pegasus/Render/GL/GLDeviceWin32.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "../Source/Pegasus/Render/GL/GLExtensions.h"
#include "Pegasus/Render/RenderContextConfig.h"
#include "Pegasus/Render/RenderContext.h"

#pragma comment(lib, "opengl32")


static const char* PEGASUS_INIT_WND_CLASSNAME = "InitWindow_PegasusEngine";
static int gIsGlewInit = false;

static PIXELFORMATDESCRIPTOR gPixelFormat = {
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

static int gOglContextAttributes[] = {
                            WGL_CONTEXT_DEBUG_BIT_ARB, 1,
                            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                            WGL_CONTEXT_MINOR_VERSION_ARB , 3,
                            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                            0, 0};

namespace Pegasus
{
namespace Render
{

static void InitializeOpenGl(HDC drawContext);
static void InitGlew();

GLDeviceWin32::GLDeviceWin32(const DeviceConfig& config, Alloc::IAllocator * allocator)
: IDevice(config, allocator)
{
    //Register a new window class
    WNDCLASSEX windowClass;

    // Set up our window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc; // Message pump callback
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = (HINSTANCE)config.mModuleHandle;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = PEGASUS_INIT_WND_CLASSNAME;
    windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register our window class with the OS
    if (!RegisterClassEx(&windowClass))
    {
        PG_FAILSTR("Failed to register the init window class!");
    }

    HWND windowHandle = CreateWindowEx(
                                  0,
                                  PEGASUS_INIT_WND_CLASSNAME,
                                  "PEGASUS__initWindow",
                                  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_SYSMENU,
                                  0, 0,
                                  100, 100,
                                  NULL,
                                  NULL,
                                  (HINSTANCE) config.mModuleHandle,
                                  (LPVOID) this);
    if (windowHandle == NULL)
    {
        DWORD e = GetLastError();
        PG_FAILSTR("Unable to create window!");
    }
    else
    {
        PG_LOG('WNDW', "Initial Window handle accquired");
    }

    InitializeOpenGl(GetDC(windowHandle));

    DestroyWindow(windowHandle);
    UnregisterClass(PEGASUS_INIT_WND_CLASSNAME, (HINSTANCE)config.mModuleHandle);
}

GLDeviceWin32::~GLDeviceWin32()
{
}

PIXELFORMATDESCRIPTOR* GLDeviceWin32::GetPixelFormatDescriptor()
{
    return &gPixelFormat;
}

int * GLDeviceWin32::GetContextAttributes()
{
    return gOglContextAttributes;
}

void InitializeOpenGl(HDC drawContext)
{
    int nPixelFormat = ChoosePixelFormat(drawContext, &gPixelFormat);

    // Setup pixel format for backbuffer
    SetPixelFormat(drawContext, nPixelFormat, &gPixelFormat);
    HGLRC renderContextHandle = wglCreateContext(drawContext);
    wglMakeCurrent(drawContext, renderContextHandle);

    PG_LOG('OGL_', "Startup context created");

    // init glew properly
    InitGlew();

    //Unbind
    wglMakeCurrent(drawContext, NULL);
    
    // Destroy fake context
    wglDeleteContext(renderContextHandle);
    PG_LOG('OGL_', "Startup context destroyed");
}

void InitGlew()
{
    //Only allow initialization once
    if (gIsGlewInit == false)
    {

        // Write some temporary debugging information
        // declaration of this class initializes glew
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

        gIsGlewInit = true;
    }
}

//! platform implementation of device
IDevice * IDevice::CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
{
    return PG_NEW(allocator, -1, "Device", Pegasus::Alloc::PG_MEM_PERM) GLDeviceWin32(config, allocator);
}

}
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
