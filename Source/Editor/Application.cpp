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
    //PG_ASSERTSTR(!fileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    mFileName = fileName;
}

//----------------------------------------------------------------------------------------

void Application::SetViewport(/**index,*/ ViewportWidget * viewportWidget)
{
    //PG_ASSERTSTR(viewportWidget != nullptr, "Invalid viewport widget set for an application");
    mViewportWidget = viewportWidget;
}

//----------------------------------------------------------------------------------------

void Application::run()
{
    //PG_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    if (mFileName.isEmpty())
    {
        emit(LoadingError(ERROR_INVALID_FILE_NAME));
        return;
    }

    //PG_ASSERTSTR(mViewportWidget != nullptr, "Invalid viewport widget set for an application");
    if (mViewportWidget == nullptr)
    {
        emit(LoadingError(ERROR_INVALID_VIEWPORT));
        return;
    }

    //! \todo Load the DLL. Return ERROR_FILE_NOT_FOUND, ERROR_INVALID_APPLICATION, ERROR_INVALID_INTERFACE if required

    // Signal the success of the loading
    emit(LoadingSucceeded());

    //! \todo Run the main loop
}
