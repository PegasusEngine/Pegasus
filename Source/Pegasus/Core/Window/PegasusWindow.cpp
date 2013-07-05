/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusWindow.cpp
//! \author	David Worsham
//! \date	4th July 2013
//! \brief	Class for a single window in a Pegasus application.

//== Includes ====
#include "Pegasus/Core/Window/PegasusWindow.h"
#include <windows.h>
//#include "Pegasus/Libs/GLEW/glew.h"
//#include "Pegasus/Libs/GLEW/wglew.h"

//#if PEGASUS_DEV
//#pragma comment (lib, "glew32.lib")
//#elif PEGASUS_REL
//#pragma comment (lib, "glew32s.lib")
//#else
//#error The GLEW library needs to be linked in this profile.
//#endif

//== Forward Declarations ====

//== Implementation ====
#ifdef __cplusplus
extern "C" {
#endif

namespace Pegasus {
namespace Core {

// Internal functions for Windows message handling
static LRESULT CALLBACK StartupWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

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

// Class/Window names for Pegasus windows
static const char* PEGASUS_WND_CLASSNAME = "PegasusEngine";
static const char* PEGASUS_WND_STARTUPCLASSNAME = "PegasusEngine_Startup";
static const char* PEGASUS_WND_WNDNAME = "PegasusEngine";
static const char* PEGASUS_WND_STARTUPWNDNAME = "PegasusEngine_Startup";

//! Config-based constructor.
//! \param config Configuration structure used to create this window.
Window::Window(const Window::WindowConfigPrivate& config)
{
    // Do the Win32 setup
    CreateWindowInternal(config);
}

//----------------------------------------------------------------------------------------

//! Destructor.
Window::~Window()
{
    // Destroy window
    DestroyWindow((HWND) mHWND);
}

//----------------------------------------------------------------------------------------

//! Internal helper to create the window.
//! \param config Configuration structure used to create this window.
void Window::CreateWindowInternal(const Window::WindowConfigPrivate& config)
{
    WNDCLASSEX windowClass;
    HWND windowHandle;

    // Set up our window class
    if (config.mIsStartupWindow)
    {
        // startup window
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = StartupWndProc; // Message pump callback
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = (HINSTANCE) config.mHINSTANCE;
        windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = PEGASUS_WND_STARTUPCLASSNAME;
        windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    }
    else
    {
        // Regular window
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WndProc; // Message pump callback
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = (HINSTANCE) config.mHINSTANCE;
        windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = PEGASUS_WND_CLASSNAME;
        windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    }

    // Register our window class with the OS
    if (!RegisterClassEx(&windowClass))
    {
        //! \todo Assert or log here
        return;
    }

    // Create window
    if (config.mIsStartupWindow)
    {
        // Startup window
        windowHandle = CreateWindowEx(NULL,
                                      PEGASUS_WND_STARTUPCLASSNAME,
                                      PEGASUS_WND_STARTUPWNDNAME,
                                      WS_OVERLAPPEDWINDOW | WS_SYSMENU,
                                      100, 100,
                                      960, 540,
                                      NULL,
                                      NULL,
                                      (HINSTANCE) config.mHINSTANCE,
                                      NULL);
    }
    else
    {
        // Normal window
        windowHandle = CreateWindowEx(NULL,
                                      PEGASUS_WND_CLASSNAME,
                                      PEGASUS_WND_WNDNAME,
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
                                      100, 100,
                                      960, 540,
                                      NULL,
                                      NULL,
                                      (HINSTANCE) config.mHINSTANCE,
                                      NULL);
    }

    // cache handle
    mHWND = (PG_HWND) windowHandle;
}

//----------------------------------------------------------------------------------------

//! Windows callback for the dummy startup window.  Fired every time a message is
//! delivered to the window.
//! \param hwnd Handle to the window.
//! \param message Message code.
//! \param wParam Message params high.
//! \param lParam Message params low.
//! \return Result status.
//! \note The only purpose of this handler/window is to initialize the OGL 1.1 context
//!       so that we can get the extension addresses.
LRESULT CALLBACK StartupWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE: // Window is being created
        {
            /*
            // Cache global context
            gHDC = GetDC(hwnd);
    
            // Setup pixel format for backbuffer
            int nPixelFormat = ChoosePixelFormat(gHDC, &gPixelFormat);
            SetPixelFormat(gHDC, nPixelFormat, &gPixelFormat);
    
            // Make a new opengl context and link it to the window
            gRC = wglCreateContext(gHDC);
            wglMakeCurrent(gHDC, gRC);
    
            // Initialize the extensions manager, after OpenGL is initialized
            GLenum error = glewInit();
            if (error != GLEW_OK)
            {
                //! \todo Proper error handling
                    OutputDebugString("Failure to init GLEW.\n");
    
                return 0;
            }
    
            // Write some temporary debugging information
            if (GLEW_VERSION_4_3)
            {
                OutputDebugString("OpenGL 4.3 is the maximum detected profile.\n");
            }
            else if (GLEW_VERSION_3_3)
            {
                OutputDebugString("OpenGL 3.3 is the maximum detected profile.\n");
            }
            else
            {
                OutputDebugString("Error when initializing GLExtensions.\n");
            }
    
            if (glewIsSupported("GL_ARB_draw_indirect"))
            {
                OutputDebugString("GL_ARB_draw_indirect detected.\n");
            }
            else
            {
                OutputDebugString("GL_ARB_draw_indirect NOT detected.\n");
            }
            if (glewIsSupported("GL_ATI_fragment_shader"))
            {
                OutputDebugString("GL_ATI_fragment_shader detected.\n");
            }
            else
            {
                OutputDebugString("GL_ATI_fragment_shader NOT detected.\n");
            }
            if (wglewIsSupported("WGL_ARB_buffer_region"))
            {
                OutputDebugString("WGL_ARB_buffer_region detected.\n");
            }
            else
            {
                OutputDebugString("WGL_ARB_buffer_region NOT detected.\n");
            }
            if (wglewIsSupported("WGL_3DL_stereo_control"))
            {
                OutputDebugString("WGL_3DL_stereo_control detected.\n");
            }
            else
            {
                OutputDebugString("WGL_3DL_stereo_control NOT detected.\n");
            }
            */
        }
        return 0;
    case WM_DESTROY: // Window is being destroyed
        {
            /*
            wglMakeCurrent(gHDC, NULL);
            wglDeleteContext(gRC);
            */
        }
        return 0;
    default:
        break;
    }
    
    // Fall back to OS default
    return (DefWindowProc(hwnd, message, wParam, lParam));
}

//----------------------------------------------------------------------------------------

//! Windows callback for a Pegasus window.  Fired every time a message is
//! delivered to the window.
//! \param hwnd Handle to the window.
//! \param message Message code.
//! \param wParam Message params high.
//! \param lParam Message params low.
//! \return Result status.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE: // Window is being created
        {
            /*
            const int attrib[6] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 4, WGL_CONTEXT_MINOR_VERSION_ARB , 3, 0, 0};
    
            // Cache global context
            gHDC = GetDC(hwnd);
    
            // Setup pixel format for backbuffer
            int nPixelFormat = ChoosePixelFormat(gHDC, &gPixelFormat);
            SetPixelFormat(gHDC, nPixelFormat, &gPixelFormat);
    
            // Make a new opengl context and link it to the window
            gRC = wglCreateContextAttribsARB(gHDC, 0, attrib);
            wglMakeCurrent(gHDC, gRC);
            */
        }
        return 0;
    case WM_DESTROY: // Window is being destroyed
        {
            /*
            wglMakeCurrent(gHDC, NULL);
            wglDeleteContext(gRC);
            */
        }
        return 0;
    case WM_CLOSE: // Someone asked to close the window
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    
    // Fall back to OS default
    return (DefWindowProc(hwnd, message, wParam, lParam));
}

//----------------------------------------------------------------------------------------


}   // namespace Core
}   // namespace Pegasus

#ifdef __cplusplus
}
#endif
