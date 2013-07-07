/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusApp.cpp
//! \author	David Worsham
//! \date	4th July 2013
//! \brief	Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#ifndef PEGASUS_PEGASUSAPP_H
#define PEGASUS_PEGASUSAPP_H

//== Includes ====
#include "Pegasus/Core/Window/PegasusWindowDefs.h"

//== Forward Declarations ====
namespace Pegasus{
    namespace Core {
        struct WindowConfig;
        class Window;
    }
}

//== Interface ====
namespace Pegasus {

//! \class Configuration structure for a Pegasus app.
struct /*PEGASUS_SHAREDOBJ*/ ApplicationConfig
{
public:
    // basic ctor / dtor
    ApplicationConfig() : mHINSTANCE(0) {}
    ApplicationConfig(Core::PG_HINSTANCE hinst) : mHINSTANCE(hinst) {}
    ~ApplicationConfig() {}

    //! Opaque application instance
    Core::PG_HINSTANCE mHINSTANCE;
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
class /*PEGASUS_SHAREDOBJ*/ Application
{
public:
    // Ctor / dtor
    Application(const ApplicationConfig& config);
    virtual ~Application();

    // Window API
    Core::Window* AttachWindow(const Core::WindowConfig& config);
    void DetachWindow(Core::Window* wnd);

    // Update API
    int Run();
    virtual void Initialize() {}
    virtual void Shutdown() {}
    virtual void Render() {}
    //! \todo Set update mode

    //! Max number of windows per app
    static const unsigned int MAX_NUM_WINDOWS = 1;

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

    //! Application instance
    Core::PG_HINSTANCE mHINSTANCE;

    //! App windows
    Core::Window* mWindows[MAX_NUM_WINDOWS];

    //! Num app windows
    unsigned int mNumWindows;
};


}   // namespace Pegasus

#endif  // PEGASUS_PEGASUSAPP_H
