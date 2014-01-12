/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IApplicationProxy.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy interface, used by the editor and launcher to interact with an app.

#ifndef PEGASUS_SHARED_IAPPPROXY_H
#define PEGASUS_SHARED_IAPPPROXY_H

#if PEGASUS_ENABLE_PROXIES
// Forward declarations
namespace Pegasus {
    namespace App {
        struct ApplicationConfig;
        struct AppWindowConfig;
    }

    namespace Timeline {
        class ITimelineProxy;
    }
    namespace Wnd {
        struct AppWindowConfig;
        class IWindowProxy;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {

//! Proxy application interface.
class IApplicationProxy
{
public:
    //! Destructor
    virtual ~IApplicationProxy() {};


    //! Gets the MAIN window type associated with this application
    //! \return Main window class type.
    virtual const char* GetMainWindowType() const = 0;

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

    //! Gets the SECONDARY window type associated with this application
    //! \return Secondary window class type.
    virtual const char* GetSecondaryWindowType() const = 0;

#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

    //! Creates a new window attached to this application instance
    //! \param config Configuration structure for the newly created window.
    //! \return Pointer to the opened window.
    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config) = 0;

    //! Destroys a window attached to this application instance
    //! \param wnd Window to destroy.
    virtual void DetachWindow(Wnd::IWindowProxy* wnd) = 0;


    //! Inits this application
    //! \param config  Config structure for the application.
    virtual void Initialize() = 0;

    //! Shuts this application down
    virtual void Shutdown() = 0;

    //! Load the assets required to render the timeline blocks
    virtual void Load() = 0;


    //! \todo Set update mode


    // Get the timeline proxy object
    //! \return Timeline proxy object
    virtual Timeline::ITimelineProxy* GetTimeline() const = 0;
};

//----------------------------------------------------------------------------------------

//! Typedef for proxy factory function
typedef Pegasus::App::IApplicationProxy* (*CreatePegasusAppFuncPtr)(const Pegasus::App::ApplicationConfig& config);
//! Typedef for proxy factory function
typedef void (*DestroyPegasusAppFuncPtr)(Pegasus::App::IApplicationProxy* app);

}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_IAPPPROXY_H
