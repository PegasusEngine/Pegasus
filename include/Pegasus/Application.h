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

#if !PEGASUS_INCLUDE_LAUNCHER
#include "Pegasus/IApplication.h"
#endif

#include "Pegasus/WindowDefs.h"


namespace Pegasus{
    struct WindowConfig;
    class Window;
}


namespace Pegasus {

//! \class Configuration structure for a Pegasus app.
struct ApplicationConfig
{
public:
    // basic ctor / dtor
    ApplicationConfig();
    ApplicationConfig(ApplicationHandle apphandle);
    ~ApplicationConfig();

    //! Opaque application instance
    ApplicationHandle mAppHandle;
};

//! \class Building block class for a Pegasus application.
//!        OVerride Init, Shutdown, and Render to perform rendering.
//! \note To use this class, simply:
//! \note   1. instantiate it
//! \note   2. attach a window to it
//! \note   3. call Run to enter the application loop
//! \todo We need to manage the list of windows properly, with a map
//!       of window handles to windows
//! \todo A lot of stuff to handle multi-windows...
#if PEGASUS_INCLUDE_LAUNCHER
class Application
#else
class Application : public IApplication
#endif
{
public:
    // Ctor / dtor
    Application();
    virtual ~Application();

    // Window API
    Window* AttachWindow(const WindowConfig& config);
    void DetachWindow(Window* wnd);

    // Update API
    int Run();
    //! \todo Set update mode

    //! Max number of windows per app
    static const unsigned int MAX_NUM_WINDOWS = 1;

    virtual void Initialize(const ApplicationConfig& config);
    virtual void Shutdown();
    virtual void Render() = 0;

private:
    // No copies allowed
    explicit Application(const Application& other);
    Application& operator=(const Application& other);

    // App helpers
    void StartupAppInternal();
    void ShutdownAppInternal();

    //! Flag for startup context creation
    static bool sContextBootstrapped;

    //! Instance counter
    static unsigned int sNumInstances;

    //! Initialized flag
    bool mInitialized;

    //! Application instance
    ApplicationHandle mAppHandle;

    //! App windows
    Window* mWindows[MAX_NUM_WINDOWS];

    //! Num app windows
    unsigned int mNumWindows;
};


}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
