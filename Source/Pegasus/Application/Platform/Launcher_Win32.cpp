/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Launcher_Win32.cpp
//! \author Kevin Boulanger
//! \date   08th July 2013
//! \brief  Engine entry point, defined only in release mode (Windows implementation)

#if PEGASUS_INCLUDE_LAUNCHER
#if PEGASUS_PLATFORM_WINDOWS

#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"

// Typedefs for DLL entry point
extern Pegasus::Application::Application* CreateApplication();
extern void DestroyApplication(Pegasus::Application::Application* app);

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <strsafe.h>


//! Engine entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Pegasus::Application::ApplicationConfig appConfig;
    Pegasus::Application::AppWindowConfig windowConfig;
    Pegasus::Application::Application* application = NULL;
    Pegasus::Window::Window* appWindow = NULL;
    int retVal = 0;

    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Window::ModuleHandle) hInstance;

    // Initialize the application
    application = CreateApplication();
    application->Initialize(appConfig);

    // Set up windows
    windowConfig.mIsChild = false;
    windowConfig.mWidth = 960;
    windowConfig.mHeight = 540;
    appWindow = application->AttachWindow(windowConfig);

    // Run the application loop
    retVal = application->Run();

    // Tear down windows
    application->DetachWindow(appWindow);

    // Destroy the application
    application->Shutdown();
    DestroyApplication(application);

    return retVal;
}


#else
#error "This file is not supposed to be compiled on platforms other than Windows"
#endif  // PEGASUS_PLATFORM_WINDOWS

#endif  // PEGASUS_INCLUDE_LAUNCHER
