/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                  */
/*                                                                                      */
/****************************************************************************************/

//! \file   LauncherWindows.cpp
//! \author Kevin Boulanger, David Worsham
//! \date   02nd June 2013
//! \brief  Entry point of Pegasus Launcher, allowing the execution of application in dev mode
//!         (Win32 and Win64 version)

//== Includes ====
#include "Pegasus/Pegasus.h"

#if PEGASUS_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>

//== Forward Declarations ====

//== Implementation ====
//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    // Test for preprocessor.h
    //! \todo Test all configs, fix bugs, and remove this
    #if PEGASUS_ENGINE
        OutputDebugString("PEGASUS_ENGINE on\n");
    #else
        OutputDebugString("PEGASUS_ENGINE off\n");
    #endif
    
    #if PEGASUS_DEV
        OutputDebugString("PEGASUS_DEV on\n");
    #else
        OutputDebugString("PEGASUS_DEV off\n");
    #endif
    
    #if PEGASUS_REL
        OutputDebugString("PEGASUS_REL on\n");
    #else
        OutputDebugString("PEGASUS_REL off\n");
    #endif
    
    #if PEGASUS_DEBUG
        OutputDebugString("PEGASUS_DEBUG on\n");
    #else
        OutputDebugString("PEGASUS_DEBUG off\n");
    #endif
    
    #if PEGASUS_OPT
        OutputDebugString("PEGASUS_OPT on\n");
    #else
        OutputDebugString("PEGASUS_OPT off\n");
    #endif
    
    #if PEGASUS_FINAL
        OutputDebugString("PEGASUS_FINAL on\n");
    #else
        OutputDebugString("PEGASUS_FINAL off\n");
    #endif
    
    #if PEGASUS_PROFILE
        OutputDebugString("PEGASUS_PROFILE on\n");
    #else
        OutputDebugString("PEGASUS_PROFILE off\n");
    #endif
    
    #if PEGASUS_SMALL
        OutputDebugString("PEGASUS_SMALL on\n");
    #else
        OutputDebugString("PEGASUS_SMALL off\n");
    #endif

    #if PEGASUSAPP_DLL
        OutputDebugString("PEGASUSAPP_DLL on\n");
    #else
        OutputDebugString("PEGASUSAPP_DLL off\n");
    #endif

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
    if (createAppProcAddress == NULL)
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
    typedef Pegasus::IApplication * (* CreatePegasusAppFuncPtr) ();
    CreatePegasusAppFuncPtr CreatePegasusAppFunc = (CreatePegasusAppFuncPtr)createAppProcAddress;
    
    // Call the application creation function from the DLL
    Pegasus::IApplication * application = CreatePegasusAppFunc();

    //! Initialize the application
    //! \todo Init

    //! Run the application loop
    //! \todo Run loop

    //! Destroy the application
    //! \todo Destroy

/*    Pegasus::ApplicationConfig testAppConfig((Pegasus::Core::PG_HINSTANCE) hInstance);
    Pegasus::Application testApp(testAppConfig);
    Pegasus::Core::WindowConfig testWindowConfig;
    Pegasus::Core::Window* testWindow = testApp.AttachWindow(testWindowConfig);
    int retVal = 0;

    // Run app
    retVal = testApp.Run();
    testApp.DetachWindow(testWindow);

    return retVal;*/

    // Destroy the application object
    delete application;

    // Release the application library
    FreeLibrary(dllModule);

    return 0;
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
