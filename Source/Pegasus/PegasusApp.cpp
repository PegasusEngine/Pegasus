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

//== Includes ====
#include "Pegasus/PegasusApp.h"
#include "Pegasus/Core/Window/PegasusWindow.h"
#include <windows.h>

#undef AttachWindow // Silly win32

//== Forward Declarations ====

//== Implementation ====
#ifdef __cplusplus
extern "C" {
#endif

namespace Pegasus {

//! Basic constructor.
//! \param config Config structure to create this app with.
Application::Application(const ApplicationConfig& config)
    : mHINSTANCE(config.mHINSTANCE), mNumWindows(0)
{
    Core::Window* startupWindow = nullptr;

    // Init windows array
    for (unsigned int i = 0; i < MAX_NUM_WINDOWS; i++)
    {
        mWindows[i] = nullptr;
    }

    // start up the app, which creates and destroys the dummy window
    StartupAppInternal();
}

//----------------------------------------------------------------------------------------

//! Basic destructor.
Application::~Application()
{
    //! \todo Assert that no windows exist here, to alert the user of them being stupid

    // Free windows
    for (unsigned int i = 0; i < MAX_NUM_WINDOWS; i++)
    {
        if (mWindows[i] != nullptr)
        {
            DetachWindow(mWindows[i]);
        }
    }
}

//----------------------------------------------------------------------------------------

//! Creates an application window and links it to this app.
//! \param config Config object used to create the window.
//! \return The created window, or nullptr if creation failed.
Core::Window* Application::AttachWindow(const Core::WindowConfig& config)
{
    Core::Window* newWnd = nullptr;
    Core::Window::WindowConfigPrivate privConfig(config);

    // Create window
    //! \todo Assert/log on failure
    privConfig.mHINSTANCE = mHINSTANCE;
    privConfig.mIsStartupWindow = false;
    newWnd = new Core::Window(privConfig);

    // Assign it
    //! \todo Assert on windows list not full
    if (newWnd)
    {
        mWindows[mNumWindows++] = newWnd;
    }

    return newWnd;
}

//----------------------------------------------------------------------------------------

//! Destroys a window and unlinks it from this app.
//! \param wnd The window to destroy.
void Application::DetachWindow(Core::Window* wnd)
{
    //! \todo Assert window not null
    //! \todo Assert window ID in range
    delete mWindows[mNumWindows - 1];
    mWindows[mNumWindows - 1] = nullptr;
    mNumWindows--;
}

//----------------------------------------------------------------------------------------

//! Runs the application loop for this pegasus app.
//! \note This method does not return until the user closes the application.
//! \return Status code.
int Application::Run()
{
    MSG curMsg;
    bool appDone = false;

    // Init app
    Initialize();

    // Run message pump until application exits
    while(!appDone)
    {
        // Grab a message and dispatch it
        PeekMessage(&curMsg, NULL, NULL, NULL, PM_REMOVE);
        if (curMsg.message == WM_QUIT)
        {
            // Bail out, app is finished
            appDone = true;
        }
        else
        {
            // Draw stuff
            Render();

            // Dispatch it
            TranslateMessage(&curMsg); 
            DispatchMessage(&curMsg);
        }
    }

    // Shutdown app
    Shutdown();

    // wParam of WM_QUIT is the app exit code
    return curMsg.wParam;
}

//----------------------------------------------------------------------------------------

//! Starts up the application.
//! \note Creates the dummy startup window used to initialize the OGL extensions.
void Application::StartupAppInternal()
{
    Core::Window* newWnd = nullptr;
    Core::WindowConfig baseConfig;
    Core::Window::WindowConfigPrivate privConfig(baseConfig);

    // Create window and immediately destroy it
    //! \todo Assert/log on failure
    privConfig.mHINSTANCE = mHINSTANCE;
    privConfig.mIsStartupWindow = true;
    newWnd = new Core::Window(privConfig);
    delete newWnd;
}

//----------------------------------------------------------------------------------------


}   // namespace Pegasus

#ifdef __cplusplus
}
#endif
