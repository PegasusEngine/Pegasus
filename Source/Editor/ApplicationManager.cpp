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
#include "Log.h"
#include "Viewport/ViewportDockWidget.h"
#include "Viewport/ViewportWidget.h"

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
    ED_LOG("Creating the application manager");

    ED_ASSERT(editor != nullptr);
    ED_ASSERT(viewportDockWidget != nullptr);
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

    ED_ASSERT(mViewportDockWidget != nullptr);

    // Close the previous application is there is one
    if (IsApplicationOpened())
    {
        ED_LOG("Closing the previously opened application");
        CloseApplication();
    }

    // Create the application object, and set its parameters
    mApplication = new Application(this);
    mApplication->SetFile(fileName);
    //! \todo Handle multiple dock widgets and multiple viewports
    ViewportWidget * viewportWidget = mViewportDockWidget->GetViewportWidget(/**0**/);
    mApplication->SetViewportParameters(/**0,*/
                                        viewportWidget->GetWindowHandle(),
                                        viewportWidget->GetWidth(),
                                        viewportWidget->GetHeight());

    // Connect the engine messages
    qRegisterMetaType<Application::Error>("Application::Error");
	connect(mApplication, SIGNAL(LoadingError(Application::Error)), this, SLOT(LoadingError(Application::Error)));
	connect(mApplication, SIGNAL(LoadingSucceeded()), this, SLOT(LoadingSucceeded()));
	connect(mApplication, SIGNAL(finished()), this, SLOT(ApplicationFinished()));

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

void ApplicationManager::LoadingError(Application::Error error)
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

void ApplicationManager::LoadingSucceeded()
{
    ED_LOG("Application successfully loaded");

    // Connect the viewport resized message from the viewport widget
    // to the application worker thread. A queued connection is used since we have
    // to cross the thread boundaries
    //! \todo Handle multiple viewports
    ViewportWidget * viewportWidget = mViewportDockWidget->GetViewportWidget(/**0*/);
    connect(viewportWidget, SIGNAL(ViewportResized(int, int)),
            mApplication, SLOT(ViewportResized(int, int)),
            Qt::QueuedConnection);

    mIsApplicationRunning = true;
}

//----------------------------------------------------------------------------------------

void ApplicationManager::ApplicationFinished()
{
    ED_LOG("The application exited");
    mIsApplicationRunning = false;

    // Destroy the application object
    if (mApplication != nullptr)
    {
        delete mApplication;
        mApplication = nullptr;
    }
}
