/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ApplicationInterface.cpp
//! \author	Kevin Boulanger
//! \date	20th October 2013
//! \brief	Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#include "Application/ApplicationInterface.h"

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"

#include <QTimer>


ApplicationInterface::ApplicationInterface(Pegasus::Application::IApplicationProxy * application,
                                           Pegasus::Window::IWindowProxy * appWindow,
                                           QObject *parent)
:   QObject(parent),
    mApplication(application),
    mAppWindow(appWindow)/*,
    mAssertionBeingHandled(false)*/
    //! \todo Seems not useful anymore. Test and remove if possible
{
    ED_ASSERTSTR(application != nullptr, "Invalid application proxy given to the application interface");
    ED_ASSERTSTR(appWindow != nullptr, "Invalid application window proxy given to the application interface");

    // Create the window redrawing timer
    // (5 ms to avoid spamming the message loop when user interaction happens, but low enough to get a good framerate)
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(RedrawChildWindows()));
    mTimer->setInterval(5);
}

//----------------------------------------------------------------------------------------

ApplicationInterface::~ApplicationInterface()
{
    // No need to delete mTimer, since its parent is this, Qt take care of it
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::StartRedrawTimer()
{
    ED_ASSERT(mTimer != nullptr);
    mTimer->start();
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::StopRedrawTimer()
{
    ED_ASSERT(mTimer != nullptr);
    mTimer->stop();
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ResizeViewport(int width, int height)
{
    ED_ASSERT(mApplication != nullptr);
    ED_ASSERT(mAppWindow != nullptr);

    //! \todo Handle multiple windows
    mApplication->ResizeWindow(mAppWindow, width, height);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawChildWindows()
{
    ED_ASSERT(mApplication != nullptr);

    // Let the redrawing happen only when no assertion dialog is present
    //! \todo Seems not useful anymore. Test and remove if possible
    //if (!mAssertionBeingHandled)
    {
        mApplication->InvalidateWindows();
    }
}
