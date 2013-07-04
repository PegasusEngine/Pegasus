/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationManager.cpp
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Manager for the opened applications in the editor

#include "ApplicationManager.h"
#include "Editor.h"
#include "Viewport/ViewportDockWidget.h"
//#include "Pegasus/Core/Assertion.h"

#include <QMessageBox>


ApplicationManager::ApplicationManager(Editor * editor,
                                       ViewportDockWidget * viewportDockWidget,
                                       QObject *parent)
:   QObject(parent),
    mEditor(editor),
    mViewportDockWidget(viewportDockWidget),
    mApplication(nullptr),
    mIsApplicationRunning(false)
{
    //PG_ASSERT(editor != nullptr);
    //PG_ASSERT(viewportDockWidget != nullptr);
}

//----------------------------------------------------------------------------------------

ApplicationManager::~ApplicationManager()
{
    if (IsApplicationOpened())
    {
        CloseApplication();
    }
}

//----------------------------------------------------------------------------------------

void ApplicationManager::OpenApplication(const QString & fileName)
{
    //PG_ASSERT(mViewportDockWidget != nullptr);

    // Close the previous application is there is one
    if (IsApplicationOpened())
    {
        CloseApplication();
    }

    // Create the application object, and set its parameters
    mApplication = new Application();
    mApplication->SetFile(fileName);
    //! \todo Handle multiple dock widgets and multiple viewports
    mApplication->SetViewport(mViewportDockWidget->GetViewportDockWidget(/**0*/));

    // Connect the engine messages
    qRegisterMetaType<Application::Error>("Application::Error");
	connect(mApplication, SIGNAL(LoadingError(Application::Error)), this, SLOT(LoadingError(Application::Error)));
	connect(mApplication, SIGNAL(LoadingSucceeded()), this, SLOT(LoadingSucceeded()));
	connect(mApplication, SIGNAL(finished()), this, SLOT(ApplicationFinished()));

    // Start the application thread
    mApplication->start(QThread::LowPriority);
}

//----------------------------------------------------------------------------------------

void ApplicationManager::CloseApplication()
{
    //PG_ASSERTSTR(mApplication != nullptr, "Trying to close an application that is not opened");

    //! \todo Implement the application closing

    mApplication = nullptr;
}

//----------------------------------------------------------------------------------------

void ApplicationManager::LoadingError(Application::Error error)
{
    switch (error)
    {
        case Application::ERROR_INVALID_FILE_NAME:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unable to load an application.\nThe provided file name is invalid."));
            break;

        case Application::ERROR_FILE_NOT_FOUND:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unable to load an application.\nThe file is not found."));
            break;

        case Application::ERROR_INVALID_APPLICATION:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unable to load an application.\nThe provided file is not a Pegasus application."));
            break;

        case Application::ERROR_INVALID_INTERFACE:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unable to load an application.\nThe provided file does not have the correct interface."));
            break;

        case Application::ERROR_INVALID_VIEWPORT:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unable to load an application.\nThe provided viewport is invalid."));
            break;

        default:
            QMessageBox::critical(mEditor->centralWidget(),
                                  tr("Open Application"),
                                  tr("Unknown error."));
            break;
    }
}

//----------------------------------------------------------------------------------------

void ApplicationManager::LoadingSucceeded()
{
    mIsApplicationRunning = true;
}

//----------------------------------------------------------------------------------------

void ApplicationManager::ApplicationFinished()
{
    mIsApplicationRunning = false;

    //! \todo Call CloseApplication?
}
