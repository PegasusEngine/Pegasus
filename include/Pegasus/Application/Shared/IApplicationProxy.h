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


    //! Gets the MAIN window type associated with this application
    //! \return Main window class type.
    virtual const char* GetMainWindowType() const = 0;

    //! Creates a new window attached to this application instance.
    //! \param config Configuration structure for the newly created window.
    //! \return Pointer to the opened window.
    virtual Window::IWindowProxy* AttachWindow(const AppWindowConfig& config) = 0;

    //! Destroys a window attached to this application instance.
    //! \param wnd Window to destroy.
    virtual void DetachWindow(const Window::IWindowProxy* wnd) = 0;


    //! Inits this application.
    //! \param config  Config structure for the application.
    virtual void Initialize() = 0;

    //! Shuts this application down.
    virtual void Shutdown() = 0;

    //! \todo Set update mode

    //! Sets the current time into the timeline for this application.
    //! \param time Desired application time.
    virtual void SetAppTime(float time) = 0;
};


//! Typedef for proxy factory function
typedef Pegasus::Application::IApplicationProxy* (*CreatePegasusAppFuncPtr)(const Pegasus::Application::ApplicationConfig& config);
//! Typedef for proxy factory function
typedef void (*DestroyPegasusAppFuncPtr)(Pegasus::Application::IApplicationProxy* app);

}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
