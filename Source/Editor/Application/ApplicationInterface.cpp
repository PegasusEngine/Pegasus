/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationInterface.cpp
//! \author Karolyn Boulanger
//! \date   20th October 2013
//! \brief  Interface object used to interface with the Pegasus related messages,
//!         created in the application thread

#include "Application/ApplicationInterface.h"
#include "Application/Application.h"
#include "AssetLibrary/AssetLibraryWidget.h"
#include "CodeEditor/CodeEditorWidget.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Timeline/TimelineDockWidget.h"
#include "Widgets/PegasusDockWidget.h"

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"


ApplicationInterface::ApplicationInterface(Application * application)
:   QObject(nullptr)
,   mApplication(application)
,   mSetCurrentBeatEnqueued(false)
,   mSetCurrentBeatEnqueuedBeat(0.0f)
,   mRedrawAllViewportsForBlockMovedEnqueued(false)
//,   mAssertionBeingHandled(false)
    //! \todo Seems not useful anymore. Test and remove if possible
{
    ED_ASSERTSTR(application != nullptr, "Invalid application object given to the application interface");

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
    ED_ASSERTSTR(timelineDockWidget != nullptr, "Unable to get the timeline dock widget");

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
    connect(timelineDockWidget, SIGNAL(BlockDoubleClicked(Pegasus::Timeline::IBlockProxy*)),
            this, SLOT(PerformBlockDoubleClickedAction(Pegasus::Timeline::IBlockProxy*)), Qt::DirectConnection);
    connect(this, SIGNAL(EnqueuedBlockMoved()),
            this, SLOT(RedrawAllViewportsForBlockMoved()),
            Qt::QueuedConnection);

    // Get widgets
    AssetLibraryWidget * assetLibraryWidget = Editor::GetInstance().GetAssetLibraryWidget();
    CodeEditorWidget   * codeEditorWidget = Editor::GetInstance().GetCodeEditorWidget();
    ProgramEditorWidget* programEditor = Editor::GetInstance().GetProgramEditorWidget();

    // Connect the texture editor messages through queued connections
    TextureEditorDockWidget * textureEditorDockWidget = Editor::GetInstance().GetTextureEditorDockWidget();
    if (textureEditorDockWidget != nullptr)
    {
        connect(textureEditorDockWidget, SIGNAL(GraphChanged()),
                this, SLOT(RedrawTextureEditorPreview()),
                Qt::QueuedConnection);
    }

    mAssetIoMessageController = new AssetIOMessageController(mApplication->GetApplicationProxy());
    mSourceIoMessageController = new SourceIOMessageController(mApplication->GetApplicationProxy());
    mProgramIoMessageController = new ProgramIOMessageController(mApplication->GetApplicationProxy());
    mSourceCodeEventListener = new SourceCodeManagerEventListener();
    
    const QVector<PegasusDockWidget*>& widgets = Editor::GetInstance().GetWidgets();
    for (int i = 0; i < widgets.size(); ++i)
    {
        PegasusDockWidget* widget = widgets[i];

        connect(widget, SIGNAL(OnSendAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message)),
                this, SLOT(ForwardAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message)), Qt::QueuedConnection);
    
        connect(mAssetIoMessageController, SIGNAL(SignalPostMessage(PegasusDockWidget*, AssetIOMessageController::Message::IoResponseMessage)),
                widget,   SLOT(ReceiveAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message::IoResponseMessage)), Qt::QueuedConnection);
        
    }

    //From render to ui
    connect(mAssetIoMessageController, SIGNAL(SignalUpdateNodeViews()),
            assetLibraryWidget, SLOT(UpdateUIItemsLayout()), Qt::QueuedConnection); 

    connect(mAssetIoMessageController, SIGNAL(SignalOpenProgram(Pegasus::Shader::IProgramProxy*)),
            programEditor, SLOT(RequestOpenProgram(Pegasus::Shader::IProgramProxy*)), Qt::QueuedConnection); 

    connect(mAssetIoMessageController, SIGNAL(SignalOpenCode(Pegasus::Core::ISourceCodeProxy*)),
            codeEditorWidget,   SLOT(RequestOpen(Pegasus::Core::ISourceCodeProxy*)), Qt::QueuedConnection); 

    //<------  Source IO Controller -------->//
    //From ui to render
    connect(codeEditorWidget, SIGNAL(SendSourceIoMessage(SourceIOMessageController::Message)),
            this, SLOT(ForwardSourceIoMessage(SourceIOMessageController::Message)), Qt::QueuedConnection);

    //from render to ui
    connect(mSourceIoMessageController, SIGNAL(SignalRedrawViewports()),
            this, SLOT(RedrawAllViewports()), Qt::DirectConnection);

    connect(mSourceIoMessageController, SIGNAL(SignalCompilationRequestEnded()),
            assetLibraryWidget, SLOT(UpdateUIItemsLayout()), Qt::QueuedConnection);

    connect(mSourceIoMessageController, SIGNAL(SignalCompilationRequestEnded()),
            assetLibraryWidget, SLOT(EnableProgramShaderViews()), Qt::QueuedConnection);

    connect(mSourceIoMessageController, SIGNAL(SignalCompilationRequestEnded()),
            codeEditorWidget, SLOT(CompilationRequestReceived()), Qt::QueuedConnection);

    //from ui to ui
    connect(codeEditorWidget, SIGNAL(RequestCompilationBegin()),
            assetLibraryWidget, SLOT(DisableProgramShaderViews()));

    //<------  Program IO Controller -------->//
    connect(mProgramIoMessageController, SIGNAL(SignalRedrawViewports()),
            this, SLOT(RedrawAllViewports()), Qt::DirectConnection);
    connect(programEditor, SIGNAL(SendProgramIoMessage(ProgramIOMessageController::Message)),
            this, SLOT(ForwardProgramIoMessage(ProgramIOMessageController::Message)));
    connect(mProgramIoMessageController, SIGNAL(SignalUpdateProgramView()),
            programEditor, SLOT(SyncUiToProgram()));
    connect(mProgramIoMessageController, SIGNAL(SignalUpdateProgramView()),
            assetLibraryWidget, SLOT(UpdateUIItemsLayout()));

    //<-------- Connect event listeners to app ------------>
    Pegasus::App::IApplicationProxy* appProxy = application->GetApplicationProxy();
    appProxy->GetShaderManagerProxy()->RegisterEventListener( mSourceCodeEventListener );
    appProxy->GetTimelineManagerProxy()->RegisterEventListener( mSourceCodeEventListener );

    //<-------- Connect event listener to widgets --------->
    connect(mSourceCodeEventListener, SIGNAL(OnCompilationError(CodeUserData*,int,QString)),
        codeEditorWidget, SLOT(SignalCompilationError(CodeUserData*,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnLinkingEvent(QString,int)),
        codeEditorWidget, SLOT(SignalLinkingEvent(QString,int)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnCompilationBegin(CodeUserData*)),
        codeEditorWidget, SLOT(SignalCompilationBegin(CodeUserData*)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnCompilationEnd(QString)),
        codeEditorWidget, SLOT(SignalCompilationEnd(QString)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnBlessUserData(CodeUserData*)),
        codeEditorWidget, SLOT(BlessUserData(CodeUserData*)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnUnblessUserData(CodeUserData*)),
        codeEditorWidget, SLOT(UnblessUserData(CodeUserData*)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnSignalSaveSuccess()),
        codeEditorWidget, SLOT(SignalSavedFileSuccess()), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnSignalSavedFileError(int, QString)),
        codeEditorWidget, SLOT(SignalSavedFileIoError(int,QString)), Qt::QueuedConnection);
    
    connect(codeEditorWidget, SIGNAL(RequestSafeDeleteUserData(CodeUserData*)),
        mSourceCodeEventListener, SLOT(SafeDestroyUserData(CodeUserData*)), Qt::QueuedConnection);
}

//----------------------------------------------------------------------------------------

ApplicationInterface::~ApplicationInterface()
{
    delete mSourceIoMessageController;
    delete mAssetIoMessageController;
    delete mProgramIoMessageController;
    delete mSourceCodeEventListener;
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
        Pegasus::Timeline::ITimelineManagerProxy * timeline = mApplication->GetTimelineProxy();
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
    Pegasus::Timeline::ITimelineManagerProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    // Set the timeline beat with the last enqueued beat rather than the first one
    timeline->GetCurrentTimeline()->SetCurrentBeat(mSetCurrentBeatEnqueuedBeat);

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

void ApplicationInterface::TogglePlayMode(bool enabled)
{
    Pegasus::Timeline::ITimelineManagerProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    if (enabled)
    {
        // When enabling the play mode, switch the timeline play mode to real-time
        timeline->GetCurrentTimeline()->SetPlayMode(Pegasus::Timeline::PLAYMODE_REALTIME);

        // Request the rendering of the first frame
        RedrawAllViewports();

        // At this point, the play mode is still enabled, so inform the linked objects
        // that will request a refresh of the viewports
        emit ViewportRedrawnInPlayMode(timeline->GetCurrentTimeline()->GetCurrentBeat());
    }
    else
    {
        // When disabling the play mode, switch the timeline play mode to stopped
        timeline->GetCurrentTimeline()->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RequestFrameInPlayMode()
{
    Pegasus::Timeline::ITimelineManagerProxy * timeline = mApplication->GetTimelineProxy();
    ED_ASSERT(timeline != nullptr);

    // Request the rendering of the viewport for the current beat
    RedrawAllViewports();

    // We are still in play mode, inform the linked objects that will request a refresh of the viewports
    emit ViewportRedrawnInPlayMode(timeline->GetCurrentTimeline()->GetCurrentBeat());
}


//----------------------------------------------------------------------------------------


void ApplicationInterface::PerformBlockDoubleClickedAction(Pegasus::Timeline::IBlockProxy* blockProxy)
{
    Pegasus::Core::ISourceCodeProxy* sourceCode = blockProxy->GetScript();
    if (sourceCode != nullptr)
    {
        ED_ASSERT(sourceCode->GetUserData() != nullptr);
        if (sourceCode->GetUserData() != nullptr)
        {
            CodeEditorWidget * codeEditorWidget = Editor::GetInstance().GetCodeEditorWidget();
            codeEditorWidget->show();
            codeEditorWidget->activateWindow();
            codeEditorWidget->RequestOpen(sourceCode);
        }
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardAssetIoMessage(PegasusDockWidget* sender, AssetIOMessageController::Message msg)
{
    mAssetIoMessageController->OnRenderThreadProcessMessage(sender, msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardSourceIoMessage(SourceIOMessageController::Message msg)
{
    mSourceIoMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardProgramIoMessage(ProgramIOMessageController::Message msg)
{
    mProgramIoMessageController->OnRenderThreadProcessMessage(msg);
}
