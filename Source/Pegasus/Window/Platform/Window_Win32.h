/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window_Win32.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Platform-specific window impl, for Windows.

#ifndef PEGASUS_WND_WINDOWWIN32_H
#define PEGASUS_WND_WINDOWWIN32_H

#if PEGASUS_PLATFORM_WINDOWS
#include "..\Source\Pegasus\Window\IWindowImpl.h"
#include "Pegasus\Window\Shared\WindowConfig.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Pegasus {
namespace Wnd {

//! Windows specific implementation for a Winodw
class WindowImpl_Win32 : public IWindowImpl
{
public:
    //! Constructor
    //! \param config Config struct used for this impl.
    //! \param messageHandler Message handler object for this impl.
    WindowImpl_Win32(const WindowConfig& config, IWindowMessageHandler* messageHandler);

    //! Destructor
    ~WindowImpl_Win32();


    // IWindowImpl interface
    virtual WindowHandle GetHandle() const { return (WindowHandle) mHWND; }
    virtual void Resize(unsigned int width, unsigned int height);

private:
    // No copies allowed
    PG_DISABLE_COPY(WindowImpl_Win32);


    //! Structure for handling a return tuple from WndProc
    struct HandleMessageReturn
    {
        HandleMessageReturn() : retcode(0), handled(false) {}
        ~HandleMessageReturn() {}


        unsigned int retcode;
        bool handled;
    };


    //! Registers window classes with the system
    //! \param Handle for the application. 
    static void RegisterWindowClass(HINSTANCE handle);

    //! Unregisters window classes with the system
    //! \param Handle for the application.
    static void UnregisterWindowClass(HINSTANCE handle);

    //! Checks if the window classes have been registered or not
    //! \return Registration status.
    static bool IsWindowClassRegistered(HINSTANCE handle);


    //! Internal helper to create the window.
    //! \param config Configuration structure used to create this window.
    void WindowImpl_Win32::Internal_CreateWindow(const WindowConfig& config);

    //! CBT callback for hooking window-specific data
    //! Called before the window is created, to provide its address for the WndProc later.
    //! \param nCode Message code.
    //! \param wParam Message params high.
    //! \param lParam Message params low.
    //! \return Result status. 
    static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

    //! Windows callback for a Pegasus window
    //! Fired every time a message is delivered to the window.
    //! \param hwnd Handle to the window.
    //! \param message Message code.
    //! \param wParam Message params high.
    //! \param lParam Message params low.
    //! \return Result status.
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    //! Windows callback for a Pegasus window
    //! Fired every time a message is delivered to the window.
    //! \param message Message code.
    //! \param wParam Message params high.
    //! \param lParam Message params low.
    //! \return Result status.
    HandleMessageReturn HandleMessage(unsigned int message, unsigned int* wParam, unsigned long* lParam);


    HMODULE mModule; //!< Module that owns this window
    HWND mHWND; //!< Window handle
    IWindowMessageHandler* mHandler; //!< MEssage handler object
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_PLATFORM_WINDOWS
#endif  // PEGASUS_WND_WINDOWWIN32_H
