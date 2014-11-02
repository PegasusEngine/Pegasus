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
#include <QItemSelectionModel>

AssetLibraryWidget::AssetLibraryWidget(QWidget * parent, CodeEditorWidget * editorWidget)
: QDockWidget(parent), mCodeEditorWidget(editorWidget)
{
    mProgramTreeModel = new ProgramTreeModel(this);
    mProgramSelectionModel = new QItemSelectionModel(mProgramTreeModel);

    mShaderListModel = new SourceCodeListModel(this);
    mShaderListSelectionModel = new QItemSelectionModel(mShaderListModel);

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

    connect(mSourceCodeManagerEventListener, SIGNAL(CompilationResultsChanged()),
        this, SLOT(UpdateUIItemsLayout()), Qt::QueuedConnection);

    //Queued connections, from events than come directly from the app
    connect(mSourceCodeManagerEventListener, SIGNAL(OnCompilationError(void*,int,QString)),
        mCodeEditorWidget, SLOT(SignalCompilationError(void*,int,QString)), Qt::QueuedConnection);

    connect(mSourceCodeManagerEventListener, SIGNAL(OnLinkingEvent(void*,QString,int)),
        mCodeEditorWidget, SLOT(SignalLinkingEvent(void*,QString,int)), Qt::QueuedConnection);

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

    ui.ProgramEditButton->setDisabled(!activation);
    ui.ShaderEditorButton->setDisabled(!activation);
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

            ActivateButtons(true);
        }
    }
}

void AssetLibraryWidget::UpdateUIItemsLayout()
{
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::InitializeInternalUserData()
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
                CodeUserData * newUserData = new CodeUserData(code);                   
                code->SetUserData(newUserData);
            } 

            for (int i = 0; i < shaderManager->GetProgramCount(); ++i)
            {
                Pegasus::Shader::IProgramProxy * program = shaderManager->GetProgram(i);
                CodeUserData * newUserData = new CodeUserData(program);
                program->SetUserData(newUserData);
            } 
            
            //send compilation request to all code
            mCodeEditorWidget->CompileCode(-1);
        }
        
    }
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
                delete code->GetUserData();
                code->SetUserData(nullptr);
            } 

            for (int i = 0; i < shaderManager->GetProgramCount(); ++i)
            {
                Pegasus::Shader::IProgramProxy * program = shaderManager->GetProgram(i);
                delete program->GetUserData();
                program->SetUserData(nullptr);
            } 
        }
    }
}

//----------------------------------------------------------------------------------------

void AssetLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    ActivateButtons(false);

    UninitializeInternalUserData();
}

