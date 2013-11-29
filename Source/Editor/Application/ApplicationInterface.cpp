/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationInterface.cpp
//! \author Kevin Boulanger
//! \date   20th October 2013
//! \brief  Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#include "Application/ApplicationInterface.h"
#include "Application/Application.h"

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"

#include <QTimer>


ApplicationInterface::ApplicationInterface(Application * application, QObject * parent)
:   QObject(parent),
    mApplication(application)/*,
    mAssertionBeingHandled(false)*/
    //! \todo Seems not useful anymore. Test and remove if possible
{
    ED_ASSERTSTR(application != nullptr, "Invalid application object given to the application interface");

    // Connect the viewport widget resized messages to the windows in the application worker thread.
    // A queued connection is used since we have to cross the thread boundaries
    //! \todo Add support for other window types
    for (unsigned int vt = 0; vt < /*NUM_VIEWPORT_TYPES*/2; ++vt)
    {
        ViewportWidget * viewportWidget = Editor::GetInstance().GetViewportWidget(ViewportType(VIEWPORTTYPE_FIRST + vt));
        if (viewportWidget != nullptr)
        {
            connect(viewportWidget, SIGNAL(ViewportResized(ViewportType, int, int)),
                    this, SLOT(ResizeViewport(ViewportType, int, int)),
                    Qt::QueuedConnection);
        }
    }

    // Create the window redrawing timer
    // (5 ms to avoid spamming the message loop when user interaction happens, but low enough to get a good framerate)
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(RedrawMainViewport()));
    mTimer->setInterval(5);
}

//----------------------------------------------------------------------------------------

ApplicationInterface::~ApplicationInterface()
{
    // No need to delete mTimer, since its parent is this, Qt takes care of it
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

void ApplicationInterface::ResizeViewport(ViewportType viewportType, int width, int height)
{
    Pegasus::Wnd::IWindowProxy * viewportWindow = mApplication->GetWindowProxy(viewportType);
    ED_ASSERT(viewportWindow != nullptr);

    viewportWindow->Resize(width, height);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawMainViewport()
{
    Pegasus::Wnd::IWindowProxy * mainViewportWindow = mApplication->GetWindowProxy(VIEWPORTTYPE_MAIN);
    ED_ASSERT(mainViewportWindow != nullptr);

    // Let the redrawing happen only when no assertion dialog is present
    //! \todo Seems not useful anymore. Test and remove if possible
    //if (!mAssertionBeingHandled)
    {
        mainViewportWindow->Refresh();
    }
}
