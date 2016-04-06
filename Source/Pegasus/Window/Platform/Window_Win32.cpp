/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window_Win32.cpp
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Platform-specific window impl, for Windows.

#if PEGASUS_PLATFORM_WINDOWS
#include "../Source/Pegasus/Window/Platform/Window_Win32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "Pegasus/Window/IWindowComponent.h"

namespace Pegasus {
namespace Wnd {

// Class/Window names for Pegasus windows
static const char* PEGASUS_WND_CLASSNAME = "PegasusEngine";
static const char* PEGASUS_WND_WNDNAME = "PegasusEngine";

static int gPegasusWinClassReferences = 0;

//----------------------------------------------------------------------------------------

IWindowImpl* IWindowImpl::CreateImpl(const WindowConfig& config, Alloc::IAllocator* alloc, IWindowMessageHandler* messageHandler)
{
    return PG_NEW(alloc, -1, "Window platform impl", Pegasus::Alloc::PG_MEM_PERM) WindowImpl_Win32(config, messageHandler);
}

//----------------------------------------------------------------------------------------

void IWindowImpl::DestroyImpl(IWindowImpl* impl, Alloc::IAllocator* alloc)
{
    PG_DELETE(alloc, impl);
}

//----------------------------------------------------------------------------------------

WindowImpl_Win32::WindowImpl_Win32(const WindowConfig& config, IWindowMessageHandler* messageHandler)
    : mHandler(messageHandler), mIsMainWindow(false)
{
    mModule = reinterpret_cast<HMODULE>(config.mDevice->GetConfig().mModuleHandle);
    // Register window class if need be
    if (gPegasusWinClassReferences == 0)
    {
        PG_ASSERT(!WindowImpl_Win32::IsWindowClassRegistered(mModule));
        WindowImpl_Win32::RegisterWindowClass(mModule);
    }

    // Create the window
    Internal_CreateWindow(config);
    ++gPegasusWinClassReferences;
}

//----------------------------------------------------------------------------------------

WindowImpl_Win32::~WindowImpl_Win32()
{
        // Destroy our window
    DestroyWindow(mHWND);
    --gPegasusWinClassReferences;
    
        // Unregister window class if need be
    if (gPegasusWinClassReferences == 0)
    {
        PG_ASSERT(WindowImpl_Win32::IsWindowClassRegistered(mModule));

        WindowImpl_Win32::UnregisterWindowClass(mModule);

        PG_ASSERT(!WindowImpl_Win32::IsWindowClassRegistered(mModule));
    }

}

//----------------------------------------------------------------------------------------

void WindowImpl_Win32::Resize(unsigned int width, unsigned int height)
{
    // Tell Windows to resize it
    SetWindowPos(mHWND, 0,
                 0, 0, 
                 width, height,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//----------------------------------------------------------------------------------------

void WindowImpl_Win32::RegisterWindowClass(HINSTANCE handle)
{
    WNDCLASSEX windowClass;

    // Set up our window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc; // Message pump callback
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = handle;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = PEGASUS_WND_CLASSNAME;
    windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register our window class with the OS
    if (!RegisterClassEx(&windowClass))
    {
        PG_FAILSTR("Failed to register window class!");
        return;
    }

    PG_LOG('WNDW', "Window class registered");
}

//----------------------------------------------------------------------------------------

void WindowImpl_Win32::UnregisterWindowClass(HINSTANCE handle)
{
    if (!UnregisterClass(PEGASUS_WND_CLASSNAME, handle))
    {
        PG_FAILSTR("Failed to unregister window class!");
        return;
    }

    PG_LOG('WNDW', "Window class unregistered");
}

//----------------------------------------------------------------------------------------

bool WindowImpl_Win32::IsWindowClassRegistered(HINSTANCE handle)
{
    WNDCLASSEX windowClass;

    return (GetClassInfoEx(handle, PEGASUS_WND_CLASSNAME, &windowClass) != 0);
}

//----------------------------------------------------------------------------------------

void WindowImpl_Win32::Internal_CreateWindow(const WindowConfig& config)
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
    hCBTHook = SetWindowsHookEx(WH_CBT, WindowImpl_Win32::CBTProc, 0, GetCurrentThreadId());
    if (hCBTHook == NULL)
    {
        PG_FAILSTR("Unable to register CBT hook!");
        return;
    }
    windowHandle = CreateWindowEx(0,
                                  PEGASUS_WND_CLASSNAME,
                                  PEGASUS_WND_WNDNAME,
                                  windowStyle,
                                  config.mIsChild ? 0 : 100, config.mIsChild ? 0 : 100,
                                  windowWidth, windowHeight,
                                  config.mIsChild ? (HWND)config.mParentWindowHandle : NULL,
                                  NULL,
                                  (HINSTANCE) mModule,
                                  (LPVOID) this);
    if (windowHandle == NULL)
    {
        DWORD err = GetLastError();
        PG_FAILSTR("Unable to create window! %d(%X)", err, err);
    }
    else
    {
        PG_LOG('WNDW', "Window handle accquired");
    }
    if (!UnhookWindowsHookEx(hCBTHook))
    {
        PG_FAILSTR("Unable to unregister CBT hook!");
        return;
    }
}

//----------------------------------------------------------------------------------------

//! CBT callback for hooking window-specific data
//! Called before the window is created, to provide its address for the WndProc later.
//! \param nCode Message code.
//! \param wParam Message params high.
//! \param lParam Message params low.
//! \return Result status. 
LRESULT CALLBACK WindowImpl_Win32::CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Hook the window before any messages are sent to it
    if (nCode == HCBT_CREATEWND)
    {
        LPCREATESTRUCT windowCreateStruct;
        WindowImpl_Win32* pThis = NULL;

        // Get the create params
        windowCreateStruct = ((CBT_CREATEWND*) lParam)->lpcs;
        pThis = (WindowImpl_Win32*) windowCreateStruct->lpCreateParams;

        // Some other bogus messages can pass NULL here
        if (pThis != NULL)
        {
            // Set up the window handle
            pThis->mHWND = (HWND) wParam;
            SetWindowLongPtr(pThis->mHWND, GWL_USERDATA, (LONG_PTR) pThis);

            PG_LOG('WNDW', "Firing CBT Hook");
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
LRESULT CALLBACK WindowImpl_Win32::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowImpl_Win32* pWnd = (WindowImpl_Win32*) GetWindowLongPtr(hwnd, GWL_USERDATA);
    WindowImpl_Win32::HandleMessageReturn wndProcReturn;

    PG_ASSERTSTR(pWnd != nullptr, "Window pointer is null in WndProc, this is bad!");

    // Call 
    wndProcReturn = pWnd->HandleMessage(message, (unsigned int*) wParam, (unsigned long*) lParam);

    // Fall back to OS default if needed
    if (!wndProcReturn.handled)
    {
        return (DefWindowProc(hwnd, message, wParam, lParam));
    }

    return wndProcReturn.retcode;
}

#if PEGASUS_ENABLE_PROXIES

static IWindowComponent::MouseButton TranslateButton(unsigned int msg)
{
    switch(msg)
    {
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        return IWindowComponent::MouseButton_Center;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        return IWindowComponent::MouseButton_Left;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:    
        return IWindowComponent::MouseButton_Right;
    case WM_MOUSEMOVE:
        return IWindowComponent::MouseButton_None;
    }

    return IWindowComponent::MouseButton_None;
}

static bool TranslateIsDown(unsigned int msg)
{
    switch(msg)
    {
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        return true;
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:    
    case WM_LBUTTONUP:
        return false;
    }

    return false;
}

#endif

//----------------------------------------------------------------------------------------

WindowImpl_Win32::HandleMessageReturn WindowImpl_Win32::HandleMessage(unsigned int message, unsigned int* wParam, unsigned long* lParam)
{
    HandleMessageReturn ret;

    switch(message)
    {
    case WM_CREATE: // Window is being created
        mHandler->OnCreate((Pegasus::Os::WindowHandle) mHWND);
        ret.handled = true; ret.retcode = 0;
        break;
    case WM_DESTROY: // Window is being destroyed
        mHandler->OnDestroy();
        ret.handled = true; ret.retcode = 0;
        if (mIsMainWindow)
        {
            //bail entire app if the main window is closed. Post the message.
            PostQuitMessage(0);
        }
        break;
    case WM_PAINT: // Someone requested a redraw of the window
#if PEGASUS_ENABLE_ASSERT
        if (!Core::AssertionManager::GetInstance()->IsAssertionBeingHandled())
#endif
        {
            PAINTSTRUCT paint;

            BeginPaint(mHWND, &paint);
            mHandler->OnRepaint(); // Render a frame
            EndPaint(mHWND, &paint);
        }

        // Letting the default message handler being called to allow the assertion dialog box
        // to not freeze the application thread (the window needs an extra paint call to unfreeze)
        ret.handled = false; ret.retcode = 0;
        break;
    case WM_SIZE: // Someone asked to resize the window
        mHandler->OnResize(LOWORD(lParam), HIWORD(lParam));
        ret.handled = true; ret.retcode = 0;
        break;
#if PEGASUS_ENABLE_PROXIES
    case WM_MOUSEMOVE:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:    
    case WM_LBUTTONDBLCLK:
        {
            RECT rect;
            GetWindowRect(mHWND, &rect);
            IWindowComponent::MouseButton button = TranslateButton(message);
            bool isDown = TranslateIsDown(message);
            float x = (float)(GET_X_LPARAM(lParam)) / (float)(rect.left - rect.right);
            float y = (float)(GET_Y_LPARAM(lParam)) / (float)(rect.bottom - rect.top);
            x = x < 0.0f ? -1.0f*x : x;
            y = y < 0.0f ? -1.0f*y : y;
            mHandler->OnMouseEvent(button, isDown, x, y);
        }
    
#endif
    default:
        break;
    }
    
    // Fall back to OS default
    return ret;
}


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
