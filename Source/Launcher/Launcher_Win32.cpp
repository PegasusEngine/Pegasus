/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                  */
/*                                                                                      */
/****************************************************************************************/

//! \file   Launcher_Win32.cpp
//! \author Kevin Boulanger, David Worsham
//! \date   02nd June 2013
//! \brief  Entry point of Pegasus Launcher, allowing the execution of application in dev mode
//!         (Win32 and Win64 version)

#include "Pegasus/Preprocessor.h"

#if PEGASUS_PLATFORM_WINDOWS

#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>


//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Pegasus::Application::ApplicationConfig appConfig;
    Pegasus::Application::AppWindowConfig windowConfig;
    Pegasus::Application::IApplicationProxy* application = NULL;
    Pegasus::Window::IWindowProxy* appWindow = NULL;
    int retVal = 0;

    // Check the command line for the name of the app to load.
    // It needs to be at least 5 characters long (x.dll)
    if ((lpCmdLine == NULL) || (strnlen_s(lpCmdLine, 8) < 5))
    {
        MessageBox(NULL,
                   TEXT("Invalid command line for the launcher.\nSyntax: Launcher.exe <AppName>.dll"),
                   TEXT("Invalid command line"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 0;
    }

    // Try to load the DLL
    HMODULE dllModule = LoadLibrary(lpCmdLine);
    if (dllModule == NULL)
    {
        // Get the error code
        const DWORD error = GetLastError();

        // Format the error message for the dialog box
        LPVOID messageBuffer;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&messageBuffer,
                      0,
                      NULL);
        LPTSTR displayBuffer = (LPTSTR)LocalAlloc(LMEM_ZEROINIT,
                                                  (lstrlen((LPCTSTR)messageBuffer) + 64) * sizeof(TCHAR));
        StringCchPrintf(displayBuffer,
                        LocalSize(displayBuffer) / sizeof(TCHAR),
                        TEXT("Unable to load the application DLL:\nError %d: %s"), 
                        error,
                        messageBuffer);
        LocalFree(messageBuffer);

        // Show the error dialog box
        MessageBox(NULL,
                   displayBuffer,
                   TEXT("Application loading"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);

        // Exit
        LocalFree(displayBuffer);
        return error;
    }

    // Retrieve the entry point of the application DLL
    FARPROC createAppProcAddress = GetProcAddress(dllModule, "CreatePegasusApp");
    FARPROC destroyAppProcAddress = GetProcAddress(dllModule, "DestroyPegasusApp");
    if (createAppProcAddress == NULL || destroyAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        MessageBox(NULL,
                   TEXT("The loaded DLL is not a valid Pegasus application."),
                   TEXT("Application loading"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 0;
    }

    // Cast the procedure into the actual entry point function
    //! \todo Make the function pointer a declaration in IApplication.h?
    Pegasus::Application::CreatePegasusAppFuncPtr CreatePegasusAppFunc = (Pegasus::Application::CreatePegasusAppFuncPtr) createAppProcAddress;
    Pegasus::Application::DestroyPegasusAppFuncPtr DestroyPegasusAppFunc = (Pegasus::Application::DestroyPegasusAppFuncPtr) destroyAppProcAddress;
    
    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Window::ModuleHandle) hInstance;

    // Initialize the application
    application = CreatePegasusAppFunc();
    application->Initialize(appConfig);

    //! Set up windows
    appWindow = application->AttachWindow(windowConfig);

    //! Run the application loop
    retVal = application->Run();

    //! Tear down windows
    application->DetachWindow(appWindow);

    // Destroy the application
    application->Shutdown();
    DestroyPegasusAppFunc(application);

    // Release the application library
    FreeLibrary(dllModule);

    return retVal;
}

// unsigned int gFrameCount = 0;
// float gClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
//void Render()
//{
//    // Update clear color once per second
//    if (gFrameCount % 60 == 0)
//    {
//        gClearColor[0] = (((float) rand()) / ((float) RAND_MAX));
//        gClearColor[1] = (((float) rand()) / ((float) RAND_MAX));
//        gClearColor[2] = (((float) rand()) / ((float) RAND_MAX));
//    }
//    gFrameCount++;
//
//    // Clear screen
//    glClearColor(gClearColor[0], gClearColor[1], gClearColor[2], gClearColor[3]);
//    glClearDepth(0.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // Present
//    glFlush();
//    SwapBuffers(gHDC);
//}

#else
#error "This file is not supposed to be compiled on platforms other than Windows"
#endif  // PEGASUS_PLATFORM_WINDOWS
