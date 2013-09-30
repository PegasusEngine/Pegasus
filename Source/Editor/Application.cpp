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
#include "Pegasus/Application.h"
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

#else
#error "Implement the loading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS

    // Cast the procedure into the actual entry point function
    //! \todo Make the function pointer a declaration in IApplication.h?
    typedef Pegasus::IApplication * (* CreatePegasusAppFuncPtr) ();
    CreatePegasusAppFuncPtr CreatePegasusAppFunc = (CreatePegasusAppFuncPtr)createAppProcAddress;
    
    // Call the application creation function from the DLL
    Pegasus::IApplication * application = CreatePegasusAppFunc();

    //! \todo Check that the version of the interface is correct

    //! \todo Call the initialization function of the interface

    // Signal the success of the loading
    emit(LoadingSucceeded());

    //! \todo Run the main loop

    // Destroy the application object
    delete application;

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
