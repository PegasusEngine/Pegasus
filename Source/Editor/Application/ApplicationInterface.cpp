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
#include "Viewport/ViewportWidget.h"
#include "Viewport/ViewportDockWidget.h"

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "MessageControllers/TimelineIOMessageController.h"
#include "MessageControllers/GraphIOMessageController.h"
#include "MessageControllers/WindowIOMessageController.h"
#include "MessageControllers/ProgramIOMessageController.h"
#include "MessageControllers/SourceIOMessageController.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/PropertyGridIOMessageController.h"


ApplicationInterface::ApplicationInterface(Application * application)
:   QObject(nullptr)
,   mApplication(application)
,   mAssertionBeingHandled(false)
{
    ED_ASSERTSTR(application != nullptr, "Invalid application object given to the application interface");


    // Connect the timeline widget messages through queued connections
    TimelineDockWidget * timelineDockWidget = Editor::GetInstance().GetTimelineDockWidget();
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

        connect(dockWidget, SIGNAL(OnSendAssetIoMessage(PegasusDockWidget*, AssetIOMCMessage)),
                this, SLOT(ForwardAssetIoMessage(PegasusDockWidget*, AssetIOMCMessage)),
                Qt::QueuedConnection);
    
        connect(mAssetIoMessageController, SIGNAL(SignalPostMessage(PegasusDockWidget*, AssetIOMCMessage::IoResponseMessage)),
                dockWidget, SLOT(ReceiveAssetIoMessage(PegasusDockWidget*, AssetIOMCMessage::IoResponseMessage)),
                Qt::QueuedConnection);
        
        connect(dockWidget, SIGNAL(OnSendGraphIoMessage(GraphIOMCMessage)),
                this, SLOT(ForwardGraphIoMessage(GraphIOMCMessage)),
                Qt::QueuedConnection);

        connect(dockWidget, SIGNAL(OnSendPropertyGridIoMessage(PropertyGridIOMCMessage)),
                this,   SLOT(ForwardPropertyGridIoMessage(PropertyGridIOMCMessage)),
				Qt::QueuedConnection);

        connect(dockWidget, SIGNAL(OnSendTimelineIoMessage(TimelineIOMCMessage)),
                this,   SLOT(ForwardTimelineIoMessage(TimelineIOMCMessage)),
				Qt::QueuedConnection);

        connect(dockWidget, SIGNAL(OnRequestRedrawAllViewports()),
                this, SLOT(RedrawAllViewports()));
    }

    //connect messages for viewports
    const QVector<ViewportWidget*>& viewportWidgets = editor.GetViewportWidgets();
    for (int i = 0; i < viewportWidgets.size(); ++i)
    {
        ViewportWidget* viewportWidget = viewportWidgets[i];
        connect(viewportWidget, SIGNAL(OnSendWindowIoMessage(WindowIOMCMessage)),
                this, SLOT(ForwardWindowIoMessage(WindowIOMCMessage)),
                Qt::QueuedConnection);

        connect(viewportWidget, SIGNAL(OnSendPropertyGridIoMessage(PropertyGridIOMCMessage)),
                this, SLOT(ForwardPropertyGridIoMessage(PropertyGridIOMCMessage)),
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
    connect(mTimelineMessageController, SIGNAL(NotifyRedrawAllViewports()),
            this, SLOT(RedrawAllViewports()));
    connect(mTimelineMessageController, SIGNAL(NotifyMasterScriptState(bool, QString)),
            timelineDockWidget, SLOT(OnShowActiveTimelineButton(bool, QString)));
    connect(mWindowIoMessageController, SIGNAL(RedrawFrame()),
            this, SLOT(RedrawAllViewports()),
			Qt::QueuedConnection);

    //<------  Source IO Controller -------->//
    //From ui to render
    connect(codeEditorWidget, SIGNAL(SendSourceIoMessage(SourceIOMCMessage)),
            this, SLOT(ForwardSourceIoMessage(SourceIOMCMessage)),
            Qt::QueuedConnection);

    connect(timelineDockWidget->GetObserver(), SIGNAL(RequestNewFrameInPlayMode(TimelineIOMessageObserver*, AssetInstanceHandle)),
            mTimelineMessageController, SLOT(RequestNewFrameInPlayMode(TimelineIOMessageObserver*, AssetInstanceHandle)),
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
    connect(programEditor, SIGNAL(SendProgramIoMessage(ProgramIOMCMessage)),
            this, SLOT(ForwardProgramIoMessage(ProgramIOMCMessage)));
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
    if (mAssertionBeingHandled)
        return;

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
    if (mAssertionBeingHandled)
        return;

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
    if (mAssertionBeingHandled)
        return;

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

void ApplicationInterface::ForwardAssetIoMessage(PegasusDockWidget* sender, AssetIOMCMessage msg)
{
    mAssetIoMessageController->OnRenderThreadProcessMessage(sender, msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardSourceIoMessage(SourceIOMCMessage msg)
{
    mSourceIoMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardProgramIoMessage(ProgramIOMCMessage msg)
{
    mProgramIoMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardWindowIoMessage(WindowIOMCMessage msg)
{
    mWindowIoMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardPropertyGridIoMessage(PropertyGridIOMCMessage msg)
{
    mPropertyGridMessageController->OnRenderThreadProcessMessage(msg);
}

//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardGraphIoMessage(GraphIOMCMessage msg)
{
    mGraphMessageController->OnRenderThreadProcessMessage(msg);
}
//----------------------------------------------------------------------------------------

void ApplicationInterface::ForwardTimelineIoMessage(TimelineIOMCMessage msg)
{
    mTimelineMessageController->OnRenderThreadProcessMessage(msg);
}

