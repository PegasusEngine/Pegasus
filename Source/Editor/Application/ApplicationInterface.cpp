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
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"


ApplicationInterface::ApplicationInterface(Application * application, QObject * parent)
:   QObject(parent),
    mApplication(application)/*,
    mAssertionBeingHandled(false)*/
    //! \todo Seems not useful anymore. Test and remove if possible
{
    ED_ASSERTSTR(application != nullptr, "Invalid application object given to the application interface");

    // Set the timeline play mode to stopped by default
    Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);
    timeline->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);

    // Connect the viewport widget resized messages to the windows in the application worker thread.
    // A queued connection is used since we have to cross the thread boundaries
    //! \todo Add support for other window types
    //! \todo Restore support for the second viewport (buffer sharing)
    for (unsigned int vt = 0; vt < /*NUM_VIEWPORT_TYPES*/1; ++vt)
    {
        ViewportWidget * viewportWidget = Editor::GetInstance().GetViewportWidget(ViewportType(VIEWPORTTYPE_FIRST + vt));
        if (viewportWidget != nullptr)
        {
            connect(viewportWidget, SIGNAL(ViewportResized(ViewportType, int, int)),
                    this, SLOT(ResizeViewport(ViewportType, int, int)),
                    Qt::QueuedConnection);
        }
    }

    // Connect the timeline widget messages through queued connections
    TimelineDockWidget * timelineDockWidget = Editor::GetInstance().GetTimelineDockWidget();
    if (timelineDockWidget != nullptr)
    {
        connect(timelineDockWidget, SIGNAL(PlayModeToggled(bool)),
                this, SLOT(TogglePlayMode(bool)),
                Qt::QueuedConnection);

        connect(this, SIGNAL(ViewportRedrawnInPlayMode(float)),
                application, SLOT(UpdateUIAndRequestFrameInPlayMode(float)),
                Qt::QueuedConnection);

        connect(application, SIGNAL(FrameRequestedInPlayMode()),
                this, SLOT(RequestFrameInPlayMode()),
                Qt::QueuedConnection);

        connect(timelineDockWidget, SIGNAL(BeatUpdated(float)),
                this, SLOT(SetCurrentBeat(float)),
                Qt::QueuedConnection);
    }
    else
    {
        ED_FAILSTR("Unable to get the timeline dock widget");
    }
}

//----------------------------------------------------------------------------------------

ApplicationInterface::~ApplicationInterface()
{
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

//----------------------------------------------------------------------------------------

void ApplicationInterface::TogglePlayMode(bool enabled)
{
    Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    if (enabled)
    {
        // When enabling the play mode, switch the timeline play mode to real-time
        timeline->SetPlayMode(Pegasus::Timeline::PLAYMODE_REALTIME);

        // Request the rendering of the first frame
        //! \todo Handle multiple viewports
        RedrawMainViewport();

        // At this point, the play mode is still enabled, so inform the linked objects
        // that will request a refresh of the viewports
        emit ViewportRedrawnInPlayMode(timeline->GetCurrentBeat());
    }
    else
    {
        // When disabling the play mode, switch the timeline play mode to stopped
        timeline->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RequestFrameInPlayMode()
{
    Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    // Request the rendering of the viewport for the current beat
    //! \todo Handle multiple viewports and if the main viewport is in real-time mode
    RedrawMainViewport();

    // We are still in play mode, inform the linked objects that will request a refresh of the viewports
    emit ViewportRedrawnInPlayMode(timeline->GetCurrentBeat());
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::SetCurrentBeat(float beat)
{
    Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    timeline->SetCurrentBeat(beat);

    //! \todo Handle multiple viewports and if the main viewport is in real-time mode
    RedrawMainViewport();
}
