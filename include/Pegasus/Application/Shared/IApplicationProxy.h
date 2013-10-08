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
