/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.h
//! \author David Worsham
//! \date   04th July 2013
//! \brief  Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#ifndef PEGASUS_APPLICATION_H
#define PEGASUS_APPLICATION_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/IWindowRegistry.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Window/IWindowContext.h"

// Forward declarations
namespace Pegasus {
    namespace Application {
        class AppWindowManager;
    }
    namespace Window {
        struct WindowConfig;
        class Window;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Application {

//! Building block class for a Pegasus application
//! Override Init, Shutdown, and Render to perform rendering.
//! \note To use this class, simply:
//! \note   1. instantiate it
//! \note   2. attach a window to it
//! \note   3. call Run to enter the application loop.
//! \todo Multi-application support
class Application : public Window::IWindowContext
{
public:
    //! Constructor
    //! \param config Configuration struct for this Application.
    Application(const ApplicationConfig& config);

    //! Destructor
    virtual ~Application();


    //! Get the name of this app
    //! \return Application name.
    virtual const char* GetAppName() const = 0;


    //! Initializes this app
    virtual void Initialize();

    //! Shut this app down
    virtual void Shutdown();


    //! Gets the window registry for this app, to register window types
    //! \return Window registry interface.
    IWindowRegistry* GetWindowRegistry();

    //! Attaches a new window to this app
    //! \param appWindowConfig Config structure for the window.
    //! \return The new window.
    //! \note Returns nullptr on failure.
    Window::Window* AttachWindow(const AppWindowConfig& appWindowConfig);

    //! Detaches a window from this app
    //! \param wnd The window to detach.
    void DetachWindow(Window::Window* wnd);
    //! \todo Set update mode

    //! Sets the current app time
    //! \param time New app time.
    inline void SetAppTime(float time) { mAppTime = time; }

    //! Gets the current app time
    //! \return Current app time.
    inline float GetAppTime() const { return mAppTime; }


    // IWindowContext interface
    virtual Io::IOManager* GetIOManager() { return mIoManager; }

private:
    // No copies allowed
    PG_DISABLE_COPY(Application);

    //! Internal helper to start up the app
    void StartupAppInternal();
    //! Internal handler to shutdown the app
    void ShutdownAppInternal();


    bool mInitialized; //!< Initialized flag
    ApplicationConfig mConfig; //!< Cached config object
    AppWindowManager* mWindowManager; //!< Window manager
    Io::IOManager* mIoManager; //!< IO manager
    float mAppTime; //!< Current app time
};

}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
