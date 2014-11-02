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
#include "AssetLibrary/AssetLibraryWidget.h"
#include "CodeEditor/CodeEditorWidget.h"

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"


ApplicationInterface::ApplicationInterface(Application * application, QObject * parent)
:   QObject(parent)
,   mApplication(application)
,   mSetCurrentBeatEnqueued(false)
,   mSetCurrentBeatEnqueuedBeat(0.0f)
,   mRedrawAllViewportsForBlockMovedEnqueued(false)
//,   mAssertionBeingHandled(false)
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
    for (unsigned int vt = 0; vt < /*NUM_VIEWPORT_TYPES*/3; ++vt)
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
                this, SLOT(RequestSetCurrentBeatAfterBeatUpdated(float)));
        connect(this, SIGNAL(EnqueuedBeatUpdated()),
                this, SLOT(SetCurrentBeat()),
                Qt::QueuedConnection);

        connect(timelineDockWidget, SIGNAL(BlockMoved()),
                this, SLOT(RequestRedrawAllViewportsAfterBlockMoved()));
        connect(this, SIGNAL(EnqueuedBlockMoved()),
                this, SLOT(RedrawAllViewportsForBlockMoved()),
                Qt::QueuedConnection);
    }
    else
    {
        ED_FAILSTR("Unable to get the timeline dock widget");
    }

    // Connect the asset library widget and the shader editor widget through queued connections
    AssetLibraryWidget * assetLibraryWidget = Editor::GetInstance().GetAssetLibraryWidget();
    if (assetLibraryWidget != nullptr)
    {
        CodeEditorWidget * codeEditorWidget = assetLibraryWidget->GetCodeEditorWidget();
        if (codeEditorWidget  != nullptr)
        {
            connect(codeEditorWidget , SIGNAL(RequestCodeCompilation(int)),
                    this, SLOT(ReceiveCompilationRequest(int)),
                    Qt::QueuedConnection);
        }
        else
        {
            ED_FAILSTR("Unable to get the code editor widget");
        }
    }
    else
    {
        ED_FAILSTR("Unable to get the asset library dock widget");
    }

    // Connect the texture editor messages through queued connections
    TextureEditorDockWidget * textureEditorDockWidget = Editor::GetInstance().GetTextureEditorDockWidget();
    if (textureEditorDockWidget != nullptr)
    {
        connect(textureEditorDockWidget, SIGNAL(GraphChanged()),
                this, SLOT(RedrawTextureEditorPreview()),
                Qt::QueuedConnection);
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

bool ApplicationInterface::RedrawMainViewport(bool updateTimeline)
{
    if (Editor::GetInstance().GetMainViewportDockWidget()->isVisible())
    {
        Pegasus::Wnd::IWindowProxy * mainViewportWindow = mApplication->GetWindowProxy(VIEWPORTTYPE_MAIN);
        ED_ASSERT(mainViewportWindow != nullptr);

        // Let the redrawing happen only when no assertion dialog is present
        //! \todo Seems not useful anymore. Test and remove if possible
        //if (!mAssertionBeingHandled)
        {
            mainViewportWindow->Refresh(updateTimeline);
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

bool ApplicationInterface::RedrawSecondaryViewport(bool updateTimeline)
{
    if (Editor::GetInstance().GetSecondaryViewportDockWidget()->isVisible())
    {
        Pegasus::Wnd::IWindowProxy * secondaryViewportWindow = mApplication->GetWindowProxy(VIEWPORTTYPE_SECONDARY);
        ED_ASSERT(secondaryViewportWindow != nullptr);

        // Let the redrawing happen only when no assertion dialog is present
        //! \todo Seems not useful anymore. Test and remove if possible
        //if (!mAssertionBeingHandled)
        {
            secondaryViewportWindow->Refresh(updateTimeline);
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawAllViewports()
{
    // Redraw the main viewport, updating the timeline
    const bool mainRedrawn = RedrawMainViewport(true);

    // If the main viewport has been redrawn, skip the timeline update for the secondary window.
    // Otherwise, update the timeline.
    const bool secondaryRedrawn = RedrawSecondaryViewport(!mainRedrawn);

    // If no viewport has been redrawn, at least update the timeline so play mode does not get stuck
    if (!mainRedrawn && !secondaryRedrawn)
    {
        Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
        if (timeline != nullptr)
        {
            //! \todo Handle music position
            timeline->Update();
        }
    }
}

//----------------------------------------------------------------------------------------

bool ApplicationInterface::RedrawTextureEditorPreview()
{
    if (Editor::GetInstance().GetTextureEditorDockWidget()->isVisible())
    {
        Pegasus::Wnd::IWindowProxy * textureEditorPreviewWindow = mApplication->GetWindowProxy(VIEWPORTTYPE_TEXTURE_EDITOR_PREVIEW);
        ED_ASSERT(textureEditorPreviewWindow != nullptr);

        // Let the redrawing happen only when no assertion dialog is present
        //! \todo Seems not useful anymore. Test and remove if possible
        //if (!mAssertionBeingHandled)
        {
            textureEditorPreviewWindow->Refresh(false);
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RequestSetCurrentBeatAfterBeatUpdated(float beat)
{
    // Always set the enqueued beat even after the enqueued flag has been set
    // so the last beat of the enqueued requests is used rather than the first one
    mSetCurrentBeatEnqueuedBeat = beat;

    if (!mSetCurrentBeatEnqueued)
    {
        // The first time that function is called, consider the request as enqueued.
        // All subsequent calls are not calling emit, until the application thread
        // takes care of the enqueued request
        mSetCurrentBeatEnqueued = true;
        emit EnqueuedBeatUpdated();
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RequestRedrawAllViewportsAfterBlockMoved()
{
    if (!mRedrawAllViewportsForBlockMovedEnqueued)
    {
        // The first time that function is called, consider the request as enqueued.
        // All subsequent calls are not calling emit, until the application thread
        // takes care of the enqueued request
        mRedrawAllViewportsForBlockMovedEnqueued = true;
        emit EnqueuedBlockMoved();
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::SetCurrentBeat()
{
    Pegasus::Timeline::ITimelineProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    // Set the timeline beat with the last enqueued beat rather than the first one
    timeline->SetCurrentBeat(mSetCurrentBeatEnqueuedBeat);

    // Since the enqueued beat has been taken into account, we can now reset the enqueued flag
    // to allow now requests to be enqueued
    mSetCurrentBeatEnqueued = false;

    RedrawAllViewports();
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawAllViewportsForBlockMoved()
{
    // Since the enqueued redraw has been taken into account, we can now reset the enqueued flag
    // to allow now requests to be enqueued
    mRedrawAllViewportsForBlockMovedEnqueued = false;

    RedrawAllViewports();
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ReceiveCompilationRequest(int id)
{
    AssetLibraryWidget * assetLibraryWidget = Editor::GetInstance().GetAssetLibraryWidget();
    if (assetLibraryWidget != nullptr)
    {
        CodeEditorWidget * codeEditorWidget = assetLibraryWidget->GetCodeEditorWidget();
        if (codeEditorWidget != nullptr)
        {
            codeEditorWidget->FlushTextEditorToCode(id);

            //refresh viewport
            mApplication->GetShaderManagerProxy()->UpdateAllPrograms();
            RedrawAllViewports();
        }
        else
        {
            ED_FAILSTR("Unable to get the code editor widget");
        }
    }
    else
    {
        ED_FAILSTR("Unable to get the asset library dock widget");
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
        RedrawAllViewports();

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
    RedrawAllViewports();

    // We are still in play mode, inform the linked objects that will request a refresh of the viewports
    emit ViewportRedrawnInPlayMode(timeline->GetCurrentBeat());
}
