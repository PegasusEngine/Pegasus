/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Application.cpp
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Worker thread to contain an application to run

#include "Application.h"
#include "Viewport/ViewportWidget.h"
#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include <stdio.h>

#if PEGASUS_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // PEGASUS_PLATFORM_WINDOWS


Application::Application(QObject *parent)
:   QThread(parent),
    mFileName(),
    mApplication(nullptr),
    mAppWindow(nullptr),
    mViewportWindowHandle(0),
    mViewportInitialWidth(128),
    mViewportInitialHeight(128)
{
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
}

//----------------------------------------------------------------------------------------

void Application::SetFile(const QString & fileName)
{
    ED_ASSERTSTR(!fileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    mFileName = fileName;
}

//----------------------------------------------------------------------------------------

void Application::SetViewportParameters(/**index,*/ Pegasus::Window::WindowHandle windowHandle,
                                        int width, int height)
{
    ED_ASSERTSTR(windowHandle != 0, "Invalid viewport window handle set for an application.");
    ED_ASSERTSTR(width > 0, "Invalid viewport initial width (%d). It must be positive.", width);
    ED_ASSERTSTR(height > 0, "Invalid viewport initial height (%d). It must be positive.", height);

    mViewportWindowHandle = windowHandle;
    mViewportInitialWidth = width;
    mViewportInitialHeight = height;
}

//----------------------------------------------------------------------------------------

void Application::run()
{
    ED_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    Pegasus::Application::ApplicationConfig appConfig;
    Pegasus::Application::AppWindowConfig windowConfig;
    int retVal = 0;

    //PG_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    if (mFileName.isEmpty())
    {
        emit(LoadingError(ERROR_INVALID_FILE_NAME));
        return;
    }

    if (mViewportWindowHandle == 0)
    {
        ED_FAILSTR("Invalid viewport window handle set for an application.");
        emit(LoadingError(ERROR_INVALID_VIEWPORT));
        return;
    }

#if PEGASUS_PLATFORM_WINDOWS

    // Load the DLL
    HMODULE dllModule = LoadLibrary(mFileName.utf16());
    if (dllModule == NULL)
    {
        emit(LoadingError(ERROR_INVALID_APPLICATION));
        return;
    }

    // Retrieve the entry point of the application DLL
    FARPROC createAppProcAddress = GetProcAddress(dllModule, "CreatePegasusApp");
    if (createAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        emit(LoadingError(ERROR_INVALID_INTERFACE));
        return;
    }
    FARPROC destroyAppProcAddress = GetProcAddress(dllModule, "DestroyPegasusApp");
    if (destroyAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        emit(LoadingError(ERROR_INVALID_INTERFACE));
        return;
    }

#else
#error "Implement the loading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS

    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Window::ModuleHandle) GetModuleHandle(NULL); // Use the handle of the Editor EXE

    // Cast the procedure into the actual entry point function
    Pegasus::Application::CreatePegasusAppFuncPtr CreatePegasusAppFunc = (Pegasus::Application::CreatePegasusAppFuncPtr) createAppProcAddress;
    Pegasus::Application::DestroyPegasusAppFuncPtr DestroyPegasusAppFunc = (Pegasus::Application::DestroyPegasusAppFuncPtr) destroyAppProcAddress;

    // Initialize the application
    mApplication = CreatePegasusAppFunc();
    mApplication->Initialize(appConfig);

    //! Set the window handler parent of the created child window
    windowConfig.mIsChild = true;
    windowConfig.mParentWindowHandle = mViewportWindowHandle;
    windowConfig.mWidth = mViewportInitialWidth;
    windowConfig.mHeight = mViewportInitialHeight;

    // Signal the success of the loading
    emit(LoadingSucceeded());

    // Set up windows
    mAppWindow = mApplication->AttachWindow(windowConfig);

    // Run the application loop
    retVal = mApplication->Run();

    // Tear down windows
    mApplication->DetachWindow(mAppWindow);

    // Destroy the application
    mApplication->Shutdown();
    DestroyPegasusAppFunc(mApplication);

#if PEGASUS_PLATFORM_WINDOWS

    // Release the application library
    FreeLibrary(dllModule);

#else
#error "Implement the unloading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS
}

//----------------------------------------------------------------------------------------

void Application::ViewportResized(int width, int height)
{
    ED_ASSERT(mApplication != nullptr);
    ED_ASSERT(mAppWindow != nullptr);

    //! \todo Handle multiple windows
    mApplication->ResizeWindow(mAppWindow, width, height);
}

//----------------------------------------------------------------------------------------

void Application::LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr)
{
    /****/
}
