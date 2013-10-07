/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.h
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Class for a single window in a Pegasus application.

#ifndef PEGASUS_WINDOW_WINDOW_H
#define PEGASUS_WINDOW_WINDOW_H

#include "Pegasus/Window/WindowDefs.h"
#include "Pegasus/Window/Shared/WindowConfig.h"


namespace Pegasus {
    namespace Render {
        class Context;
    }
}


namespace Pegasus {
namespace Window {

//! \class Class that represents an application window.
//! \todo we should create a WindowManager for multi windows
class Window
{
public:
    // Ctor / Dtor
    Window(const WindowConfig& config);
    virtual ~Window();

    // Getter
    inline WindowHandle GetHandle() const;
    inline Render::Context* GetRenderContext() const;

    // Message handling
#if PEGASUS_PLATFORM_WINDOWS
    struct HandleMessageReturn
    {
        HandleMessageReturn() : retcode(0), handled(false) {}
        ~HandleMessageReturn() {}


        unsigned int retcode;
        bool handled;
    };

    // Temp, until we make window manager
    static bool RegisterWindowClass(ModuleHandle handle);
    static bool UnregisterWindowClass(ModuleHandle handle);
    void SetHWND(WindowHandle handle) { mHWND = handle; }; //! Temp, until moved to platform specific
    HandleMessageReturn HandleMessage(unsigned int message, unsigned int* wParam, unsigned long* lParam);
#endif

private:
    // No copies allowed
    explicit Window(const Window& other);
    Window& operator=(const Window& other);

    // Helpers
    void Internal_CreateWindow(const WindowConfig& config);


    //! Cached startup window flag
    bool mIsStartupWindow;

    //! Window handle
    WindowHandle mHWND;

    //! Rendering context
    Render::Context* mRenderContext;
};


//----------------------------------------------------------------------------------------

//! Gets the handle for this window.
//! \return Window handle.
inline WindowHandle Window::GetHandle() const
{
    return mHWND;
}

//----------------------------------------------------------------------------------------

//! Gets the OGL render context for this window.
//! \return Render context.
inline Render::Context* Window::GetRenderContext() const
{
    return mRenderContext;
}

//----------------------------------------------------------------------------------------


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_WINDOW_H
