/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetLibraryWidget.cpp
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Graphics widget representing the shader library form.

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Application/ApplicationManager.h"
#include "AssetLibrary/AssetLibraryWidget.h"
#include "AssetLibrary/ProgramTreeModel.h"
#include "AssetLibrary/SourceCodeListModel.h"
#include "CodeEditor/CodeEditorWidget.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include <QItemSelectionModel>
#include <QTextDocument>

AssetLibraryWidget::AssetLibraryWidget(QWidget * parent, CodeEditorWidget * editorWidget)
: QDockWidget(parent), mCodeEditorWidget(editorWidget)
{

    mProgramTreeModel = new ProgramTreeModel(this);
    mProgramSelectionModel = new QItemSelectionModel(mProgramTreeModel);

    mShaderListModel = new SourceCodeListModel(this);
    mShaderListSelectionModel = new QItemSelectionModel(mShaderListModel);

    mBlockScriptListModel = new SourceCodeListModel(this);
    mBlockScriptListSelectionModel = new QItemSelectionModel(mBlockScriptListModel);

    mSourceCodeManagerEventListener = new SourceCodeManagerEventListener(this);

    ui.setupUi(this);


    connect(ui.ShaderEditorButton, SIGNAL(clicked()),
            this, SLOT(DispatchShaderEditButton()));

    connect(ui.ProgramEditButton, SIGNAL(clicked()),
            this, SLOT(DispatchProgramEditButton()));

    connect(ui.ShaderTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughShaderView(QModelIndex)));

    connect(ui.ProgramTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughProgramView(QModelIndex)));

    connect(ui.BlockScriptTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchTextEditorThroughBlockScriptView(QModelIndex)));

    //Queued connections, from events than come directly from the app
    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationError(void*,int,QString)),
        mCodeEditorWidget, SLOT(SignalCompilationError(void*,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnLinkingEvent(QString,int)),
        mCodeEditorWidget, SLOT(SignalLinkingEvent(QString,int)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationBegin(void*)),
        mCodeEditorWidget, SLOT(SignalCompilationBegin(void*)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationEnd(QString)),
        mCodeEditorWidget, SLOT(SignalCompilationEnd(QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnBlessUserData(void*)),
        mCodeEditorWidget, SLOT(BlessUserData(void*)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnUnblessUserData(void*)),
        mCodeEditorWidget, SLOT(UnblessUserData(void*)), Qt::QueuedConnection);

    connect(mCodeEditorWidget, SIGNAL(RequestDisableAssetLibraryUi()),
        this, SLOT(OnCompilationRedrawBegin()), Qt::DirectConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSaveSuccess()),
        mCodeEditorWidget, SLOT(SignalSavedFileSuccess()), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSavedFileError(int, QString)),
        mCodeEditorWidget, SLOT(SignalSavedFileIoError(int,QString)), Qt::QueuedConnection);
    
    connect(mCodeEditorWidget, SIGNAL(RequestSafeDeleteUserData(void*)),
        mSourceCodeManagerEventListener, SLOT(SafeDestroyUserData(void*)), Qt::QueuedConnection);

    ui.ProgramTreeView->setModel(mProgramTreeModel);
    ui.ProgramTreeView->setSelectionModel(mProgramSelectionModel);

    ui.ShaderTreeView->setModel(mShaderListModel);
    ui.ShaderTreeView->setSelectionModel(mShaderListSelectionModel);

    ui.BlockScriptTreeView->setModel(mBlockScriptListModel);
    ui.BlockScriptTreeView->setSelectionModel(mBlockScriptListSelectionModel);

    ActivateButtons(false);
}

//----------------------------------------------------------------------------------------
void AssetLibraryWidget::ActivateButtons(bool activation)
{
    //disable program buttons for now
    //! TODO pegasus can't add or remove shader elements yet. Force these buttons to be disabled
    ui.ProgramAddButton->setDisabled(/*!activation*/ true);
    ui.ProgramRemoveButton->setDisabled(/*!activation*/ true);

    //! TODO pegasus can't add or remove shader elements yet. Force these buttons to be disabled
    ui.ShaderAddButton->setDisabled(/*!activation*/ true);
    ui.ShaderRemoveButton->setDisabled(/*!activation*/ true);

    //! TODO pegasus can't add or remove shader elements yet. Force these buttons to be disabled
    ui.BlockScriptAddButton->setDisabled(/*!activation*/ true);
    ui.BlockScriptRemoveButton->setDisabled(/*!activation*/ true);

    ui.ProgramEditButton->setDisabled(!activation);
    ui.ShaderEditorButton->setDisabled(!activation);
    ui.BlockScriptEditorButton->setDisabled(!activation);
}


//----------------------------------------------------------------------------------------
void AssetLibraryWidget::DispatchShaderEditButton()
{
    if (mShaderListSelectionModel->hasSelection())
    {
        QModelIndex index = mShaderListSelectionModel->currentIndex();
        DispatchTextEditorThroughShaderView(index);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchProgramEditButton()
{
    if (mProgramSelectionModel->hasSelection())
    {
        QModelIndex index = mProgramSelectionModel->currentIndex();
        DispatchTextEditorThroughShaderView(index);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughShaderView(const QModelIndex& index)
{
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    Pegasus::Core::ISourceCodeProxy * code = mShaderListModel->Translate(index);
    CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
    mCodeEditorWidget->RequestOpen(userData);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughBlockScriptView(const QModelIndex& index)
{
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    Pegasus::Core::ISourceCodeProxy * code = mBlockScriptListModel->Translate(index);
    CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
    mCodeEditorWidget->RequestOpen(userData);
}


//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateEditorStyle()
{
    mCodeEditorWidget->OnSettingsChanged();
}
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::OnProgramLinkingEvent(void * program, QString message, int eventType)
{
    Pegasus::Shader::IProgramProxy * target = static_cast<Pegasus::Shader::IProgramProxy*>(program);
    ED_ASSERT(target != nullptr);
    CodeUserData * programUserData = static_cast<CodeUserData*>(target->GetUserData());
    if (programUserData != nullptr)
    {
        programUserData->SetIsValid(
            static_cast<Pegasus::Core::CompilerEvents::LinkingEvent::Type>(eventType) == Pegasus::Core::CompilerEvents::LinkingEvent::LINKING_SUCCESS
        );
        programUserData->SetErrorMessage(message);
        //update ui
        ui.ProgramTreeView->doItemsLayout();
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughProgramView(const QModelIndex& index)
{
    if (mProgramTreeModel->IsProgramIndex(index))
    {
    }
    else if (mProgramTreeModel->IsShaderIndex(index))
    {
        mCodeEditorWidget->show();
        mCodeEditorWidget->activateWindow();
        Pegasus::Core::ISourceCodeProxy * code = static_cast<Pegasus::Core::ISourceCodeProxy*>(mProgramTreeModel->TranslateShaderIndex(index));
        CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
        mCodeEditorWidget->RequestOpen(userData);
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::SetEnabledProgramShaderViews(bool enabled)
{
    ui.ProgramTreeView->setEnabled(enabled);
    ui.ShaderTreeView->setEnabled(enabled);
}
void AssetLibraryWidget::OnCompilationRedrawBegin()
{
    SetEnabledProgramShaderViews(false);
}

void AssetLibraryWidget::OnCompilationRedrawEnd()
{
    SetEnabledProgramShaderViews(true);
    UpdateUIItemsLayout();
}

//----------------------------------------------------------------------------------------

AssetLibraryWidget::~AssetLibraryWidget()
{
    delete mSourceCodeManagerEventListener;
    delete mProgramTreeModel;
    delete mShaderListModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateUIForAppLoaded()
{
    Application* app = Editor::GetInstance().GetApplicationManager().GetApplication();
    mCodeEditorWidget->CompileCode(-1);
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy* appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy* shaderMgrProxy = appProxy->GetShaderManagerProxy();
            ED_ASSERTSTR(shaderMgrProxy != nullptr, "Shader Manager not extracted from app");
            
            if (shaderMgrProxy != nullptr)
            {
                mShaderListModel->OnAppLoaded(shaderMgrProxy);
                ui.ShaderTreeView->doItemsLayout();
                
                mProgramTreeModel->OnAppLoaded();
                ui.ProgramTreeView->doItemsLayout();
            }

            Pegasus::Timeline::ITimelineProxy* timelineProxy = appProxy->GetTimelineProxy();
            if (timelineProxy != nullptr)
            {
                mBlockScriptListModel->OnAppLoaded(timelineProxy);
                ui.BlockScriptTreeView->doItemsLayout();
            }

            ActivateButtons(true);
        }
        ui.ShaderTreeView->setEnabled(false);
    }
}

void AssetLibraryWidget::UpdateUIItemsLayout()
{
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
    ui.BlockScriptTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    mBlockScriptListModel->OnAppDestroyed();
    ui.BlockScriptTreeView->doItemsLayout();

    ActivateButtons(false);
}

