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


namespace Pegasus {
namespace Application {

//! \class Building block class for a Pegasus application.
//!        Override Init, Shutdown, and Render to perform rendering.
//! \note To use this class, simply:
//! \note   1. instantiate it
//! \note   2. attach a window to it
//! \note   3. call Run to enter the application loop
//! \todo We need to manage the list of windows properly, with a map
//!       of window handles to windows
//! \todo A lot of stuff to handle multi-windows...
//! \todo Multi-application support
class Application
{
public:
    // Ctor / dtor
    Application(const ApplicationConfig& config);
    virtual ~Application();

    // App API
    virtual void Initialize();
    virtual void Shutdown();

    // Window API
    IWindowRegistry* GetWindowRegistry();
    Window::Window* AttachWindow(const AppWindowConfig& appWindowConfig);
    void DetachWindow(const Window::Window* wnd);
    //! \todo Set update mode

    // Update API
    inline void SetAppTime(float time) { mAppTime = time; }
    inline float GetAppTime() const { return mAppTime; }

private:
    // No copies allowed
    explicit Application(const Application& other);
    Application& operator=(const Application& other);

    // App helpers
    void StartupAppInternal();
    void ShutdownAppInternal();


    //! Initialized flag
    bool mInitialized;

    //! Application instance
    Window::ModuleHandle mModuleHandle;

    //! Window manager
    AppWindowManager* mWindowManager;

    //! Current app time
    float mAppTime;
};

}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
