/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.h
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Class for a single window in a Pegasus application.

#ifndef PEGASUS_WINDOW_H
#define PEGASUS_WINDOW_H

#include "Pegasus/WindowDefs.h"


namespace Pegasus {
    namespace Render {
        class Context;
    }
}


namespace Pegasus {

//! \class Configuration structure for a Pegasus window.
//! \todo We need actual params here
struct WindowConfig
{
public:
    // Basic ctor/dtor
    WindowConfig();
    ~WindowConfig();
};

//! \class Class that represents an application window.
//! \todo we should create a WindowManager for multi windows
class Window
{
public:
    // Dtor
    ~Window();

    // Getter
    inline WindowHandle GetHandle() const;
    inline Render::Context* GetRenderContext() const;

private:
    // No copies allowed
    explicit Window(const Window& other);
    Window& operator=(const Window& other);

    //! \class Private config structure with internal options
    struct WindowConfigPrivate
    {
    public:
        // Basic ctor/dtor
        WindowConfigPrivate(const WindowConfig& config);
        ~WindowConfigPrivate();

        //! Base config structure
        WindowConfig mBaseConfig;

        //! Opaque application instance
        ApplicationHandle mAppHandle;

        //! Internal startup window flag
        //! This indicates that the window is a dummy,
        //! for creating the initial render context
        bool mIsStartupWindow;
    };

    // Private-only ctor
    Window(const WindowConfigPrivate& config);

    // Helpers
    void CreateWindowInternal(const WindowConfigPrivate& config);


    //! Window handle
    WindowHandle mHWND;

    //! Rendering context
    Render::Context* mRenderContext;

    // Only an app can construct these
    friend class Application;
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


}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_H
