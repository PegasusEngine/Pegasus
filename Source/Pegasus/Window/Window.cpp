/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.cpp
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Class for a single window in a Pegasus application.

#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Application/Application.h"
#include <windows.h>

namespace Pegasus {
namespace Window {

// Internal functions for Windows message handling
static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Class/Window names for Pegasus windows
static const char* PEGASUS_WND_CLASSNAME = "PegasusEngine";
static const char* PEGASUS_WND_WNDNAME = "PegasusEngine";

//----------------------------------------------------------------------------------------

//! Config-based constructor.
//! \param config Configuration structure used to create this window.
Window::Window(const WindowConfig& config)
:   mWindowContext(config.mWindowContext),
    mHWND(NULL),
    mUseBasicContext(config.mUseBasicContext),
    mRenderContext(NULL),
    mWidth(config.mWidth),
    mHeight(config.mHeight)
{
    // Do the Win32 setup
    Internal_CreateWindow(config);
}

//----------------------------------------------------------------------------------------

//! Destructor.
Window::~Window()
{
    // Destroy window
    DestroyWindow((HWND) mHWND);
}

//----------------------------------------------------------------------------------------

void Window::Resize(unsigned int width, unsigned int height)
{
    // Tell Windows to resize it
    SetWindowPos((HWND)mHWND, 0,
                 0, 0, 
                 width, height,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//----------------------------------------------------------------------------------------

#if PEGASUS_PLATFORM_WINDOWS
//! Handles Win32 messages sent to this window.
//! \param message Win32 message ID.
//! \param wParam  Win32 wparam for this message.
//! \param lParam  Win32 lparam for this message.
//! \return Status code and handled flag from the message handler.
Window::HandleMessageReturn Window::HandleMessage(unsigned int message, unsigned int* wParam, unsigned long* lParam)
{
    HandleMessageReturn ret;

    switch(message)
    {
    case WM_CREATE: // Window is being created
        {
            HDC deviceContext = GetDC((HWND) mHWND);
            Render::ContextConfig contextConfig;

            // Create context
            contextConfig.mDeviceContextHandle = (Render::DeviceContextHandle) deviceContext;
            contextConfig.mStartupContext = mUseBasicContext;
            mRenderContext = PG_NEW("Render::Context", Pegasus::Memory::PG_MEM_PERM) Render::Context(contextConfig);
        }
        ret.handled = true; ret.retcode = 0;
        break;
    case WM_DESTROY: // Window is being destroyed
        {
            // Destroy context
            PG_DELETE mRenderContext;
        }
        ret.handled = true; ret.retcode = 0;
        break;
    case WM_PAINT: // Someone requested a redraw of the window
#if PEGASUS_ENABLE_ASSERT
        if (!Core::AssertionManager::GetInstance().IsAssertionBeingHandled())
#endif
        {
            PAINTSTRUCT paint;

            BeginPaint((HWND) mHWND, &paint);
            Refresh(); // Render a frame
            EndPaint((HWND) mHWND, &paint);
        }

        // Letting the default message handler being called to allow the assertion dialog box
        // to not freeze the application thread (the window needs an extra paint call to unfreeze)
        ret.handled = false; ret.retcode = 0;

        break;
    case WM_CLOSE: // Someone asked to close the window
        PostQuitMessage(0); //! \todo Is this OK at all?  Rethink...
        ret.handled = true; ret.retcode = 0;
        break;
    case WM_SIZE: // Someone asked to resize the window
        // Cache
        mWidth = LOWORD(lParam);
        mHeight = HIWORD(lParam);
        ret.handled = true; ret.retcode = 0;
        break;
    default:
        break;
    }
    
    // Fall back to OS default
    return ret;
}
#endif

//----------------------------------------------------------------------------------------

//! Internal helper to create the window.
//! \param config Configuration structure used to create this window.
void Window::Internal_CreateWindow(const WindowConfig& config)
{
    HWND windowHandle;
    HHOOK hCBTHook;
    DWORD windowStyle;

    // Compute window style
    //! \todo Add support for fullscreen mode
    //! \todo Add support for windowStyleEx
    windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    if (config.mIsChild)
    {
        windowStyle |= WS_CHILD;
    }
    else
    {
        windowStyle |= WS_OVERLAPPEDWINDOW | WS_SYSMENU;
    }
    if (config.mCreateVisible)
    {
        windowStyle |= WS_VISIBLE;
    }

    // In windowed non-child mode, adjust the rectangle of the window
    // to account for the title bar and the borders
    int windowWidth = config.mWidth;
    int windowHeight = config.mHeight;
    if (!config.mIsChild /* & !config.mIsFullscreen*/)
    {
        RECT rect;
        rect.left = 0;
        rect.right = windowWidth;
        rect.top = 0;
        rect.bottom = windowHeight;
        AdjustWindowRectEx(&rect, windowStyle, FALSE, /*windowStyleEx*/0);
        windowWidth = rect.right - rect.left;
        windowHeight = rect.bottom - rect.top;
    }

    // Create window
    hCBTHook = SetWindowsHookEx(WH_CBT, CBTProc, 0, GetCurrentThreadId());
    windowHandle = CreateWindowEx(0,
                                  PEGASUS_WND_CLASSNAME,
                                  PEGASUS_WND_WNDNAME,
                                  windowStyle,
                                  config.mIsChild ? 0 : 100, config.mIsChild ? 0 : 100,
                                  windowWidth, windowHeight,
                                  config.mIsChild ? (HWND)config.mParentWindowHandle : NULL,
                                  NULL,
                                  (HINSTANCE) config.mModuleHandle,
                                  (LPVOID) this);
    if (!UnhookWindowsHookEx(hCBTHook))
    {
        //! \todo Assert or log here
        return;
    }
}

//----------------------------------------------------------------------------------------

bool Window::RegisterWindowClass(ModuleHandle handle)
{
    WNDCLASSEX windowClass;

    // Set up our window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc; // Message pump callback
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = (HINSTANCE) handle;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = PEGASUS_WND_CLASSNAME;
    windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register our window class with the OS
    if (!RegisterClassEx(&windowClass))
    {
        //! \todo Assert or log here
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------

bool Window::UnregisterWindowClass(ModuleHandle handle)
{
    if (!UnregisterClass(PEGASUS_WND_CLASSNAME, (HINSTANCE) handle))
    {
        //! \todo Assert or log here
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------

//! CBT callback for hooking window-specific data
//! Called before the window is created, to provide its address for the WndProc later.
//! \param nCode Message code.
//! \param wParam Message params high.
//! \param lParam Message params low.
//! \return Result status. 
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Hook the window before any messages are sent to it
    if (nCode == HCBT_CREATEWND)
    {
        LPCREATESTRUCT windowCreateStruct;
        Window* pThis = NULL;

        // Get the create params
        windowCreateStruct = ((CBT_CREATEWND*) lParam)->lpcs;
        pThis = (Window*) windowCreateStruct->lpCreateParams;

        // Some other bogus messages can pass NULL here
        if (pThis != NULL)
        {
            // Set up the window handle
            pThis->SetHWND((WindowHandle) wParam);

            //! \todo Assert on valid Window pointer
            SetWindowLongPtr((HWND) wParam, GWL_USERDATA, (LONG_PTR) pThis);
        }
    }

    return 0;
}

//----------------------------------------------------------------------------------------

//! Windows callback for a Pegasus window
//! Fired every time a message is delivered to the window.
//! \param hwnd Handle to the window.
//! \param message Message code.
//! \param wParam Message params high.
//! \param lParam Message params low.
//! \return Result status.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* pWnd = (Window*) GetWindowLongPtr(hwnd, GWL_USERDATA);
    Window::HandleMessageReturn wndProcReturn;

    //! \todo Assert window pointer not null

    // Call 
    wndProcReturn = pWnd->HandleMessage(message, (unsigned int*) wParam, (unsigned long*) lParam);

    // Fall back to OS default if needed
    if (!wndProcReturn.handled)
    {
        return (DefWindowProc(hwnd, message, wParam, lParam));
    }

    return wndProcReturn.retcode;
}


}   // namespace Window
}   // namespace Pegasus
