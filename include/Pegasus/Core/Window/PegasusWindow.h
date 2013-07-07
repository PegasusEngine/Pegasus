/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusWindow.h
//! \author	David Worsham
//! \date	4th July 2013
//! \brief	Class for a single window in a Pegasus application.

#ifndef PEGASUS_CORE_PEGASUSWINDOW_H
#define PEGASUS_CORE_PEGASUSWINDOW_H

//== Includes ====
#include "Pegasus/Core/Window/PegasusWindowDefs.h"

//== Forward Declarations ====
namespace Pegasus {
    namespace Render {
        class Context;
    }
}

//== Interface ====
namespace Pegasus {
namespace Core {

//! \class Configuration structure for a Pegasus window.
//! \todo We need actual params here
struct /*PEGASUS_SHAREDOBJ*/ WindowConfig
{
public:
    // Basic ctor/dtor
    WindowConfig() {}
    ~WindowConfig() {}
};

//! \class Class that represents an application window.
//! \todo we should create a WindowManager for multi windows
class /*PEGASUS_SHAREDOBJ*/ Window
{
public:
    // Dtor
    ~Window();

    // Getter
    inline PG_HWND GetHandle() const;
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
        WindowConfigPrivate(const WindowConfig& config)
            : mBaseConfig(config), mHINSTANCE(0), mIsStartupWindow(false) {}
        ~WindowConfigPrivate() {}

        //! Base config structure
        WindowConfig mBaseConfig;

        //! Opaque application instance
        Core::PG_HINSTANCE mHINSTANCE;

        //! Internal startup window flag
        //! This indicates that the window is a dummy,
        //! for creating the inital render context
        bool mIsStartupWindow;
    };

    // Private-only ctor
    Window(const WindowConfigPrivate& config);

    // Helpers
    void CreateWindowInternal(const WindowConfigPrivate& config);


    //! Window handle
    PG_HWND mHWND;

    //! Rendering context
    Render::Context* mRenderContext;

    // Only an app can construct these
    friend class Application;
};


//----------------------------------------------------------------------------------------

//! Gets the handle for this window.
//! \return Window handle.
inline PG_HWND Window::GetHandle() const
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


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_PEGASUSWINDOW_H
