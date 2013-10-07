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
    mViewportWidget(nullptr)
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

void Application::SetViewport(/**index,*/ ViewportWidget * viewportWidget)
{
    ED_ASSERTSTR(viewportWidget != nullptr, "Invalid viewport widget set for an application.");
    mViewportWidget = viewportWidget;
}

//----------------------------------------------------------------------------------------

void Application::run()
{
    ED_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    Pegasus::Application::ApplicationConfig appConfig;
    Pegasus::Application::AppWindowConfig windowConfig;
    Pegasus::Application::IApplicationProxy* application = NULL;
    Pegasus::Window::IWindowProxy* appWindow = NULL;
    int retVal = 0;

    //PG_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    if (mFileName.isEmpty())
    {
        emit(LoadingError(ERROR_INVALID_FILE_NAME));
        return;
    }

    ED_ASSERTSTR(mViewportWidget != nullptr, "Invalid viewport widget set for an application.");
    if (mViewportWidget == nullptr)
    {
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
    application = CreatePegasusAppFunc();
    application->Initialize(appConfig);

    // Signal the success of the loading
    emit(LoadingSucceeded());

    // Set up windows
    appWindow = application->AttachWindow(windowConfig);

    // Run the application loop
    retVal = application->Run();

    // Tear down windows
    application->DetachWindow(appWindow);

    // Destroy the application
    application->Shutdown();
    DestroyPegasusAppFunc(application);

#if PEGASUS_PLATFORM_WINDOWS

    // Release the application library
    FreeLibrary(dllModule);

#else
#error "Implement the unloading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS
}

//----------------------------------------------------------------------------------------

void Application::LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr)
{
    /****/
}
