/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationManager.cpp
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Manager for the opened applications in the editor

#include "Application/ApplicationManager.h"
#include "Editor.h"
#include "Log.h"
#include "Viewport/ViewportDockWidget.h"
#include "Viewport/ViewportWidget.h"
#include "Viewport/ViewportType.h"

#include <QMessageBox>


ApplicationManager::ApplicationManager(Editor * editor, QObject *parent)
:   QObject(parent),
    mEditor(editor),
    mApplication(nullptr),
    mIsApplicationRunning(false)
{
    ED_LOG("Creating the application manager");

    ED_ASSERT(editor != nullptr);

    // Register the custom types used in the editor signals
    qRegisterMetaType<Application::Error>("Application::Error");
    qRegisterMetaType<ViewportType>("ViewportType");
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
    ED_LOG("Opening application '%s'", fileName.toLatin1().constData());

    // Close the previous application is there is one
    if (IsApplicationOpened())
    {
        ED_LOG("Closing the previously opened application");
        CloseApplication();
    }

    // Create the application object, and set its parameters
    mApplication = new Application(this);
    mApplication->SetFile(fileName);

    // Connect the engine messages
	connect(mApplication, SIGNAL(LoadingError(Application::Error)), this, SLOT(OnLoadingError(Application::Error)));
	connect(mApplication, SIGNAL(LoadingSucceeded()), this, SLOT(OnLoadingSucceeded()));
	connect(mApplication, SIGNAL(finished()), this, SLOT(OnApplicationFinished()));

    // Start the application thread
    ED_LOG("Starting the application thread");
    mApplication->start(QThread::LowPriority);
}

//----------------------------------------------------------------------------------------

void ApplicationManager::CloseApplication()
{
    ED_LOG("Closing the current application");
    ED_ASSERTSTR(mApplication != nullptr, "Trying to close an application that is not opened.");

    if (mApplication != nullptr)
    {
        //! \todo Implement the application closing

        delete mApplication;
        mApplication = nullptr;
    }
}

//----------------------------------------------------------------------------------------

void ApplicationManager::OnLoadingError(Application::Error error)
{
    const QString title(tr("Open Application"));
    QString message;
    switch (error)
    {
        case Application::ERROR_INVALID_FILE_NAME:
            message = tr("Unable to load an application.\nThe provided file name is invalid.");
            break;

        case Application::ERROR_INVALID_APPLICATION:
            message = tr("Unable to load an application.\nThe provided file is not a Pegasus application.");
            break;

        case Application::ERROR_INVALID_INTERFACE:
            message = tr("Unable to load an application.\nThe provided file does not have the correct interface.");
            break;

        case Application::ERROR_INVALID_VIEWPORT:
            message = tr("Unable to load the application.\nThe provided viewport is invalid.");
            break;

        default:
            message = tr("Unable to load an application.\nUnknown error.");
            break;
    }

    ED_LOG(message.toLatin1().constData());
    QMessageBox::warning(mEditor->centralWidget(), title, message);
}

//----------------------------------------------------------------------------------------

void ApplicationManager::OnLoadingSucceeded()
{
    ED_LOG("Application successfully loaded");

    mIsApplicationRunning = true;

    emit(ApplicationLoaded());
}

//----------------------------------------------------------------------------------------

void ApplicationManager::OnApplicationFinished()
{
    ED_LOG("The application exited");
    mIsApplicationRunning = false;

    // Destroy the application object
    if (mApplication != nullptr)
    {
        delete mApplication;
        mApplication = nullptr;
    }

    emit(ApplicationFinished());
}
