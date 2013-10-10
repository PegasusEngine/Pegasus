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

// Forward declarations
namespace Pegasus {
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
    Application();
    virtual ~Application();

    // Window API
    Window::Window* AttachWindow(const AppWindowConfig& appWindowConfig);
    void DetachWindow(const Window::Window* wnd);
    void ResizeWindow(Window::Window* wnd, int width, int height);
    //! \todo Set update mode

    // App API
    virtual void Initialize(const ApplicationConfig& config);
    virtual void Shutdown();
    int Run();

    // Render API
    virtual void Resize(const Window::Window* wnd, int width, int height);
    virtual void Render();

    //! Max number of windows per app
    static const unsigned int MAX_NUM_WINDOWS = 1;

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

    //! App windows
    Window::Window* mWindows[MAX_NUM_WINDOWS];

    //! Num app windows
    unsigned int mNumWindows;
};

}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
