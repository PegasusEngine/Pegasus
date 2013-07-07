/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusWindow.cpp
//! \author	David Worsham
//! \date	4th July 2013
//! \brief	Class for a single window in a Pegasus application.

#include "Pegasus/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include <windows.h>


namespace Pegasus {

// Internal functions for Windows message handling
static LRESULT CALLBACK StartupWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Class/Window names for Pegasus windows
static const char* PEGASUS_WND_CLASSNAME = "PegasusEngine";
static const char* PEGASUS_WND_STARTUPCLASSNAME = "PegasusEngine_Startup";
static const char* PEGASUS_WND_WNDNAME = "PegasusEngine";
static const char* PEGASUS_WND_STARTUPWNDNAME = "PegasusEngine_Startup";


//! Basic constructor.
WindowConfig::WindowConfig()
{
}

//----------------------------------------------------------------------------------------

//! Destructor.
WindowConfig::~WindowConfig()
{
}

//----------------------------------------------------------------------------------------

//! Config-based constructor.
//! \param config Configuration structure to use internally.
Window::WindowConfigPrivate::WindowConfigPrivate(const WindowConfig& config)
 : mBaseConfig(config), mAppHandle(NULL), mIsStartupWindow(false)
{
}

//----------------------------------------------------------------------------------------

//! Destructor.
Window::WindowConfigPrivate::~WindowConfigPrivate()
{
}

//----------------------------------------------------------------------------------------

//! Config-based constructor.
//! \param config Configuration structure used to create this window.
Window::Window(const Window::WindowConfigPrivate& config)
    : mRenderContext(NULL)
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
        windowClass.hInstance = (HINSTANCE) config.mAppHandle;
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
        windowClass.hInstance = (HINSTANCE) config.mAppHandle;
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
                                      (HINSTANCE) config.mAppHandle,
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
                                      (HINSTANCE) config.mAppHandle,
                                      NULL);
    }

    // cache handle
    mHWND = (WindowHandle) windowHandle;
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
            //Render::ContextConfig contextConfig((PG_HWND) hwnd);

            ////! \todo Need real allocators
            //// Create context
            //contextConfig.mStartupContext = true;
            //mRenderContext = new Render::Context(contextConfig);
        }
        return 0;
    case WM_DESTROY: // Window is being destroyed
        {
            ////! \todo Need real allocators
            //// Destroy context
            //delete mRenderContext;
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
            //Render::ContextConfig contextConfig((PG_HWND) hwnd);

            ////! \todo Need real allocators
            //// Create context
            //contextConfig.mStartupContext = false;
            //mRenderContext = new Render::Context(contextConfig);
        }
        return 0;
    case WM_DESTROY: // Window is being destroyed
        {
            ////! \todo Need real allocators
            //// Destroy context
            //delete mRenderContext;
        }
        return 0;
    case WM_PAINT: // Someone requested a redraw of the window
        //! \todo We need to call the render() method here
        break;
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


}   // namespace Pegasus
