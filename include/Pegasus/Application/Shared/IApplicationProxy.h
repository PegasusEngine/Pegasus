/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IApplicationProxy.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy interface, used by the editor and launcher to interact with an app.

#ifndef PEGASUS_SHARED_IAPPLICATIONPROXY_H
#define PEGASUS_SHARED_IAPPLICATIONPROXY_H

// Those includes are not required to compile Pegasus, but they become important
// when compiling an external application using the proxy, such as the editor
#include "Pegasus/Core/Log.h"
#include "Pegasus/Core/Assertion.h"


// Forward declarations
namespace Pegasus {
    namespace Application {
        struct ApplicationConfig;
        struct AppWindowConfig;
    }

    namespace Window {
        struct AppWindowConfig;
        class IWindowProxy;
    }
}

namespace Pegasus {
namespace Application {

//! \class Proxy application interface.
class IApplicationProxy
{
public:
    //! Destructor
    virtual ~IApplicationProxy() {};


    //! Creates a new window attached to this application instance.
    //! \param config Configuration structure for the newly created window.
    //! \return Pointer to the opened window.
    virtual Window::IWindowProxy* AttachWindow(const AppWindowConfig& config) = 0;

    //! Destroys a window attached to this application instance.
    //! \param wnd Window to destroy.
    virtual void DetachWindow(const Window::IWindowProxy* wnd) = 0;

    //! Resize a window attached to this application instance
    //! \param wnd Window to resize
    //! \param width New width in pixels of the window
    //! \param height New height in pixels of the window
    virtual void ResizeWindow(Window::IWindowProxy* wnd, int width, int height) = 0;


    //! Inits this application.
    //! \param config  Config structure for the application.
    virtual void Initialize(const ApplicationConfig& config) = 0;

    //! Shuts this application down.
    virtual void Shutdown() = 0;

    //! Runs this application.
    //! \return Return code.
    //! \note This method does not return until the user closes the application.
    virtual int Run() = 0;
    //! \todo Set update mode


#if PEGASUS_ENABLE_LOG
    //! Register the log handler of the application.
    //! This is called by the application launcher to set a callback
    //! for log messages emitted from the application itself
    //! \param handler Handler function defined in the launcher
    virtual void RegisterLogHandler(Core::LogManager::Handler handler) = 0;
#endif

#if PEGASUS_ENABLE_ASSERT
    //! Register the assertion handler of the application.
    //! This is called by the application launcher to set a callback
    //! for assertion errors emitted from the application itself
    //! \param handler Handler function defined in the launcher
    virtual void RegisterAssertionHandler(Core::AssertionManager::Handler handler) = 0;

    //! Called when a request to redraw the content of the application windows is sent
    //! \todo Temporary, we need a better way to draw, on a per-window basis
    virtual void InvalidateWindows() = 0;
#endif

    //! Sets the current time into the timeline for this application.
    //! \param time Desired application time.
    virtual void SetAppTime(float time) = 0;

    //! Renders a frame.
    virtual void Render() = 0;
};


//! Typedef for proxy factory function
typedef Pegasus::Application::IApplicationProxy* (*CreatePegasusAppFuncPtr)();
//! Typedef for proxy factory function
typedef void (*DestroyPegasusAppFuncPtr)(Pegasus::Application::IApplicationProxy* app);

}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
