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

    connect(mSourceCodeManagerEventListener, SIGNAL(CompilationResultsChanged()),
        this, SLOT(UpdateUIItemsLayout()), Qt::QueuedConnection);

    //These two have to be blocking. The destructor has to coordinate with the app, make sure
    // the editor is frozen so user can't access anything non destroyed or created.
    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalNewObject(QString)),
        this, SLOT(UpdateAssetViewLayout(QString)), Qt::BlockingQueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalDestroyObject(void*, QString)),
        this, SLOT(UpdateRemovedCodeObject(void*, QString)), Qt::BlockingQueuedConnection);

    //Queued connections, from events than come directly from the app
    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationError(void*,int,QString)),
        mCodeEditorWidget, SLOT(SignalCompilationError(void*,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnLinkingEvent(QString,int)),
        mCodeEditorWidget, SLOT(SignalLinkingEvent(QString,int)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationBegin(void*)),
        mCodeEditorWidget, SLOT(SignalCompilationBegin(void*)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationEnd(QString)),
        mCodeEditorWidget, SLOT(SignalCompilationEnd(QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSaveSuccess()),
        mCodeEditorWidget, SLOT(SignalSavedFileSuccess()), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnSignalSavedFileError(int, QString)),
        mCodeEditorWidget, SLOT(SignalSavedFileIoError(int,QString)), Qt::QueuedConnection);

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
    mCodeEditorWidget->RequestOpen(code);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::DispatchTextEditorThroughBlockScriptView(const QModelIndex& index)
{
    mCodeEditorWidget->show();
    mCodeEditorWidget->activateWindow();
    Pegasus::Core::ISourceCodeProxy * code = mBlockScriptListModel->Translate(index);
    mCodeEditorWidget->RequestOpen(code);
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
        mCodeEditorWidget->RequestOpen(code);
    }
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
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy* appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy can't be null");
        if (appProxy != nullptr)
        {
            InitializeInternalUserData();

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
    }
}

void AssetLibraryWidget::UpdateUIItemsLayout()
{
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
    ui.BlockScriptTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------
void AssetLibraryWidget::UpdateProgramUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy * shaderManager = appProxy->GetShaderManagerProxy();
            for (int i = 0; i < shaderManager->GetProgramCount(); ++i)
            {
                Pegasus::Shader::IProgramProxy * program = shaderManager->GetProgram(i);
                if (program->GetUserData() == nullptr)
                {
                    CodeUserData * newUserData = new CodeUserData(program);
                    program->SetUserData(newUserData);
                }
            } 
        }
    }
}
//----------------------------------------------------------------------------------------
void AssetLibraryWidget::UpdateShaderUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy * shaderManager = appProxy->GetShaderManagerProxy();
            for (int i = 0; i < shaderManager->GetShaderCount(); ++i)
            {
                Pegasus::Core::ISourceCodeProxy * code = static_cast<Pegasus::Core::ISourceCodeProxy*>(shaderManager->GetShader(i));
                if (code->GetUserData() == nullptr)
                {
                    CodeUserData * newUserData = new CodeUserData(code);
                    code->SetUserData(newUserData);
                    newUserData->SetDocument( new QTextDocument() );
                }
            } 
        }
    }
}
//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateBlockScriptUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Core::ISourceCodeManagerProxy* blockScripts = appProxy->GetTimelineProxy();
            for (int i = 0; i < blockScripts->GetSourceCount(); ++i)
            {
                Pegasus::Core::ISourceCodeProxy* scProxy = blockScripts->GetSource(i);
                if (scProxy->GetUserData() == nullptr)
                {
                    CodeUserData* newUserData = new CodeUserData(scProxy);
                    scProxy->SetUserData(newUserData);
                    newUserData->SetDocument( new QTextDocument() );
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::InitializeInternalUserData()
{
    UpdateProgramUserData();
    UpdateShaderUserData();
    UpdateBlockScriptUserData();
    //send compilation request to all code 
    mCodeEditorWidget->CompileCode(-1);
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UninitializeInternalUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy * shaderManager = appProxy->GetShaderManagerProxy();
            ED_ASSERTSTR(shaderManager != nullptr, "Failed retrieving shader manager");            
            for (int i = 0; i < shaderManager->GetShaderCount(); ++i)
            {
                Pegasus::Core::ISourceCodeProxy * code = static_cast<Pegasus::Core::ISourceCodeProxy*>(shaderManager->GetShader(i));
                CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
                if (userData->GetDocument() != nullptr)
                {
                    delete userData->GetDocument();
                }
                delete userData;
                code->SetUserData(nullptr);

            } 

            for (int i = 0; i < shaderManager->GetProgramCount(); ++i)
            {
                Pegasus::Shader::IProgramProxy * program = shaderManager->GetProgram(i);
                delete program->GetUserData();
                program->SetUserData(nullptr);
            } 

            Pegasus::Core::ISourceCodeManagerProxy* blockScripts = appProxy->GetTimelineProxy();
            for (int i = 0; i < blockScripts->GetSourceCount(); ++i)
            {
                Pegasus::Core::ISourceCodeProxy* scProxy = blockScripts->GetSource(i);                
                CodeUserData* userData = static_cast<CodeUserData*>(scProxy->GetUserData());
                if (userData ->GetDocument() != nullptr)
                {
                    delete userData ->GetDocument();
                }
                delete userData;
                scProxy->SetUserData(nullptr);
            }
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateAssetViewLayout(QString objName)
{
    if (objName == "Program")
    {
        UpdateProgramUserData();
        ui.ProgramTreeView->doItemsLayout();
    }
    else if (objName == "ShaderStage")
    {
        UpdateShaderUserData();
        ui.ShaderTreeView->doItemsLayout();
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateRemovedCodeObject(void* userData, QString objName)
{
    if (userData != nullptr)
    {
        //TODO: here close the UI of the code if opened
        CodeUserData* codeUserData = static_cast<CodeUserData*>(userData);
        if (codeUserData->GetSourceCode() != nullptr)
        {
            int idx = mCodeEditorWidget->FindIndex(codeUserData->GetSourceCode());
            if (idx != -1) 
            {
                mCodeEditorWidget->RequestClose(idx);
            }
        }

        delete codeUserData;
    }

    if (objName == "Program")
    {
        ui.ProgramTreeView->doItemsLayout();
    }
    else if (objName == "ShaderStage")
    {
        ui.ShaderTreeView->doItemsLayout();
    }
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

    UninitializeInternalUserData();
}

