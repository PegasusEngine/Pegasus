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
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"


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
            this, SLOT(PerformBlockDoubleClickedAction(Pegasus::Timeline::IBlockProxy*)),
            Qt::DirectConnection);
    connect(this, SIGNAL(EnqueuedBlockMoved()),
            this, SLOT(RedrawAllViewportsForBlockMoved()),
            Qt::QueuedConnection);

    // Get widgets
    AssetLibraryWidget * assetLibraryWidget = Editor::GetInstance().GetAssetLibraryWidget();
    CodeEditorWidget   * codeEditorWidget = Editor::GetInstance().GetCodeEditorWidget();
    ProgramEditorWidget* programEditor = Editor::GetInstance().GetProgramEditorWidget();

    mAssetIoMessageController = new AssetIOMessageController(mApplication->GetApplicationProxy());
    mSourceIoMessageController = new SourceIOMessageController(mApplication->GetApplicationProxy());
    mProgramIoMessageController = new ProgramIOMessageController(mApplication->GetApplicationProxy());
    mWindowIoMessageController  = new WindowIOMessageController(mApplication->GetApplicationProxy());
    mPropertyGridMessageController = new PropertyGridIOMessageController(mApplication->GetApplicationProxy(), mAssetIoMessageController);
    mGraphMessageController = new GraphIOMessageController(mApplication->GetApplicationProxy());
    mTimelineMessageController = new TimelineIOMessageController(mApplication->GetApplicationProxy());
    mSourceCodeEventListener = new SourceCodeManagerEventListener();

    //Register asset translators. These will convert from app proxies to qvariant data:
    mSourceIoMessageController->RegisterAssetIOController(mAssetIoMessageController);
    mProgramIoMessageController->RegisterAssetIOController(mAssetIoMessageController);
    mTimelineMessageController->RegisterAssetIOController(mAssetIoMessageController);
    mGraphMessageController->RegisterAssetIOController(mAssetIoMessageController);

    Editor& editor = Editor::GetInstance();
    connect(mAssetIoMessageController, SIGNAL(SignalOpenObject(AssetInstanceHandle, QString, int, QVariant)),
            &editor, SLOT(OnOpenObject(AssetInstanceHandle, QString, int, QVariant)), Qt::QueuedConnection); 
    
    const QVector<PegasusDockWidget*>& dockWidgets = editor.GetDockWidgets();
    for (int i = 0; i < dockWidgets.size(); ++i)
    {
        PegasusDockWidget* dockWidget = dockWidgets[i];

        connect(dockWidget, SIGNAL(OnSendAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message)),
                this, SLOT(ForwardAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message)),
                Qt::QueuedConnection);
    
        connect(mAssetIoMessageController, SIGNAL(SignalPostMessage(PegasusDockWidget*, AssetIOMessageController::Message::IoResponseMessage)),
                dockWidget, SLOT(ReceiveAssetIoMessage(PegasusDockWidget*, AssetIOMessageController::Message::IoResponseMessage)),
                Qt::QueuedConnection);
        
        connect(dockWidget, SIGNAL(OnSendGraphIoMessage(GraphIOMessageController::Message)),
                this, SLOT(ForwardGraphIoMessage(GraphIOMessageController::Message)),
                Qt::QueuedConnection);

        connect(dockWidget, SIGNAL(OnSendPropertyGridIoMessage(PropertyGridIOMessageController::Message)),
                this,   SLOT(ForwardPropertyGridIoMessage(PropertyGridIOMessageController::Message)),
				Qt::QueuedConnection);

        connect(dockWidget, SIGNAL(OnSendTimelineIoMessage(TimelineIOMessageController::Message)),
                this,   SLOT(ForwardTimelineIoMessage(TimelineIOMessageController::Message)),
				Qt::QueuedConnection);
    }

    //connect messages for viewports
    const QVector<ViewportWidget*>& viewportWidgets = editor.GetViewportWidgets();
    for (int i = 0; i < viewportWidgets.size(); ++i)
    {
        ViewportWidget* viewportWidget = viewportWidgets[i];
        connect(viewportWidget, SIGNAL(OnSendWindowIoMessage(WindowIOMessageController::Message)),
                this, SLOT(ForwardWindowIoMessage(WindowIOMessageController::Message)),
                Qt::QueuedConnection);

        connect(viewportWidget, SIGNAL(OnSendPropertyGridIoMessage(PropertyGridIOMessageController::Message)),
                this, SLOT(ForwardPropertyGridIoMessage(PropertyGridIOMessageController::Message)),
                Qt::QueuedConnection);
    }

    //From render to ui    
    connect(mAssetIoMessageController, SIGNAL(RedrawFrame()),
            this, SLOT(RedrawAllViewports()),
			Qt::QueuedConnection);
    connect(mTimelineMessageController, SIGNAL(NotifyRepaintTimeline()),
            timelineDockWidget, SLOT(OnRepaintTimeline()),
			Qt::QueuedConnection);
    connect(mTimelineMessageController, SIGNAL(NotifyRepaintTimeline()),
            this, SLOT(RedrawAllViewports()),
			Qt::QueuedConnection);
    connect(mTimelineMessageController, SIGNAL(NotifyMasterScriptState(bool, QString)),
            timelineDockWidget, SLOT(OnShowActiveTimelineButton(bool, QString)));
    connect(mWindowIoMessageController, SIGNAL(RedrawFrame()),
            this, SLOT(RedrawAllViewports()),
			Qt::QueuedConnection);

    //<------  Source IO Controller -------->//
    //From ui to render
    connect(codeEditorWidget, SIGNAL(SendSourceIoMessage(SourceIOMessageController::Message)),
            this, SLOT(ForwardSourceIoMessage(SourceIOMessageController::Message)),
            Qt::QueuedConnection);

    //from render to ui
    connect(mSourceIoMessageController, SIGNAL(SignalRedrawViewports()),
            this, SLOT(RedrawAllViewports()),
			Qt::DirectConnection);

    connect(mSourceIoMessageController, SIGNAL(SignalCompilationRequestEnded()),
            codeEditorWidget, SLOT(CompilationRequestReceived()),
			Qt::QueuedConnection);


    //<------  Program IO Controller -------->//
    connect(mProgramIoMessageController, SIGNAL(SignalRedrawViewports()),
            this, SLOT(RedrawAllViewports()),
            Qt::DirectConnection);
    connect(programEditor, SIGNAL(SendProgramIoMessage(ProgramIOMessageController::Message)),
            this, SLOT(ForwardProgramIoMessage(ProgramIOMessageController::Message)));
    connect(mProgramIoMessageController, SIGNAL(SignalUpdateProgramView()),
            programEditor, SLOT(SyncUiToProgram()));

    //<------- PropertyGrid IO Controller ----------->//
    connect(mPropertyGridMessageController, SIGNAL(RequestRedraw()),
            this, SLOT(RedrawAllViewports()));

    //<-------- Connect event listeners to app ------------>
    Pegasus::App::IApplicationProxy* appProxy = application->GetApplicationProxy();
    appProxy->GetShaderManagerProxy()->RegisterEventListener( mSourceCodeEventListener );
    appProxy->GetTimelineManagerProxy()->RegisterEventListener( mSourceCodeEventListener );

    //<-------- Connect event listener to widgets --------->
    connect(mSourceCodeEventListener, SIGNAL(OnCompilationError(AssetInstanceHandle,int,QString)),
        codeEditorWidget, SLOT(SignalCompilationError(AssetInstanceHandle,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnLinkingEvent(QString,int)),
            codeEditorWidget, SLOT(SignalLinkingEvent(QString,int)),
            Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnCompilationBegin(AssetInstanceHandle)),
        codeEditorWidget, SLOT(SignalCompilationBegin(AssetInstanceHandle)), Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnCompilationEnd(QString)),
            codeEditorWidget, SLOT(SignalCompilationEnd(QString)),
            Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnSignalSaveSuccess()),
            codeEditorWidget, SLOT(SignalSavedFileSuccess()),
            Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnSignalSavedFileError(int, QString)),
            codeEditorWidget, SLOT(SignalSavedFileIoError(int,QString)),
            Qt::QueuedConnection);

    connect(mSourceCodeEventListener, SIGNAL(OnTagValidity(QString, bool)),
            mAssetIoMessageController, SLOT(OnTagValidity(QString, bool)), Qt::QueuedConnection);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::DestroyAllWindows()
{
    mWindowIoMessageController->DestroyWindows();
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ConnectAssetEventListeners()
{
    mApplication->GetApplicationProxy()->GetAssetLibProxy()->SetEventListener(mAssetIoMessageController);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::DisconnectAssetEventListeners()
{
    mApplication->GetApplicationProxy()->GetAssetLibProxy()->SetEventListener(nullptr);
}

//----------------------------------------------------------------------------------------

ApplicationInterface::~ApplicationInterface()
{
    delete mSourceIoMessageController;
    delete mAssetIoMessageController;
    delete mProgramIoMessageController;
    delete mWindowIoMessageController;
    delete mPropertyGridMessageController;
    delete mGraphMessageController;
    delete mSourceCodeEventListener;
    delete mTimelineMessageController;
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawMainViewport()
{
    if (Editor::GetInstance().GetMainViewportDockWidget()->isVisible())
    {
        Pegasus::Wnd::IWindowProxy* w = Editor::GetInstance().GetMainViewportDockWidget()->GetViewportWidget()->GetWindowProxy();
        if (w != nullptr)
        {
            w->Draw();
        }
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawSecondaryViewport()
{
    if (Editor::GetInstance().GetSecondaryViewportDockWidget()->isVisible())
    {
        Pegasus::Wnd::IWindowProxy* w = Editor::GetInstance().GetSecondaryViewportDockWidget()->GetViewportWidget()->GetWindowProxy();
        if (w != nullptr)
        {
            w->Draw();
        }
    }
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::RedrawAllViewports()
{
    // Sim & Sound - update application.
    mApplication->GetApplicationProxy()->Update();

    //TODO: more generic.

    // Redraw the main viewport, updating the timeline
    RedrawMainViewport();
    
    // If the main viewport has been redrawn, skip the timeline update for the secondary window.
    // Otherwise, update the timeline.
    RedrawSecondaryViewport();

    //! Flush all the updates done by the app into the ui observers.
    mPropertyGridMessageController->FlushAllPendingUpdates();
    mGraphMessageController->FlushAllPendingUpdates();
    mAssetIoMessageController->FlushAllPendingUpdates();
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
    //TODO: make this thread safe:
    Pegasus::Core::ISourceCodeProxy* sourceCode = blockProxy->GetScript();
    if (blockProxy->GetScript() != nullptr && sourceCode->GetOwnerAsset() != nullptr)
    {
        AssetIOMessageController::Message msg(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(QString(sourceCode->GetOwnerAsset()->GetPath()));
        emit (Editor::GetInstance().GetCodeEditorWidget()->SendAssetIoMessage(msg));
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

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardWindowIoMessage(WindowIOMessageController::Message msg)
{
    mWindowIoMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardPropertyGridIoMessage(PropertyGridIOMessageController::Message msg)
{
    mPropertyGridMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardGraphIoMessage(GraphIOMessageController::Message msg)
{
    mGraphMessageController->OnRenderThreadProcessMessage(msg);
}
//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardTimelineIoMessage(TimelineIOMessageController::Message msg)
{
    mTimelineMessageController->OnRenderThreadProcessMessage(msg);
}

