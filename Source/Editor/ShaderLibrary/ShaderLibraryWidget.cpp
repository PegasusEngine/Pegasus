/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderLibraryWidget.cpp
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Graphics widget representing the shader library form.

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "Application/ApplicationManager.h"
#include "ShaderLibrary/ShaderLibraryWidget.h"
#include "ShaderLibrary/ProgramTreeModel.h"
#include "ShaderLibrary/ShaderListModel.h"
#include "ShaderLibrary/ShaderEditorWidget.h"
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include <QItemSelectionModel>

ShaderLibraryWidget::ShaderLibraryWidget(QWidget * parent)
: QDockWidget(parent)
{
    mShaderEditorWidget = new ShaderEditorWidget(nullptr);
    mShaderEditorWidget->hide();

    mProgramTreeModel = new ProgramTreeModel(this);
    mProgramSelectionModel = new QItemSelectionModel(mProgramTreeModel);

    mShaderListModel = new ShaderListModel(this);
    mShaderListSelectionModel = new QItemSelectionModel(mShaderListModel);

    mShaderManagerEventListener = new ShaderManagerEventListener(this);

    ui.setupUi(this);


    connect(ui.ShaderEditorButton, SIGNAL(clicked()),
            this, SLOT(DispatchShaderEditButton()));

    connect(ui.ProgramEditButton, SIGNAL(clicked()),
            this, SLOT(DispatchProgramEditButton()));

    connect(ui.ShaderTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchShaderEditorThroughShaderView(QModelIndex)));

    connect(ui.ProgramTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchShaderEditorThroughProgramView(QModelIndex)));

    connect(mShaderManagerEventListener, SIGNAL(CompilationResultsChanged(void*)),
        this, SLOT(UpdateUIItemsLayout(void*)), Qt::QueuedConnection);

    connect(mShaderManagerEventListener, SIGNAL(OnCompilationError(void*,int,QString)),
        mShaderEditorWidget, SLOT(SignalCompilationError(void*,int,QString)), Qt::QueuedConnection);

    ui.ProgramTreeView->setModel(mProgramTreeModel);
    ui.ProgramTreeView->setSelectionModel(mProgramSelectionModel);

    ui.ShaderTreeView->setModel(mShaderListModel);
    ui.ShaderTreeView->setSelectionModel(mShaderListSelectionModel);

    ActivateButtons(false);
}

//----------------------------------------------------------------------------------------
void ShaderLibraryWidget::ActivateButtons(bool activation)
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
void ShaderLibraryWidget::DispatchShaderEditButton()
{
    if (mShaderListSelectionModel->hasSelection())
    {
        QModelIndex index = mShaderListSelectionModel->currentIndex();
        DispatchShaderEditorThroughShaderView(index);
    }
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::DispatchProgramEditButton()
{
    if (mProgramSelectionModel->hasSelection())
    {
        QModelIndex index = mProgramSelectionModel->currentIndex();
        DispatchShaderEditorThroughProgramView(index);
    }
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::DispatchShaderEditorThroughShaderView(const QModelIndex& index)
{
    mShaderEditorWidget->show();
    mShaderEditorWidget->activateWindow();
    Pegasus::Shader::IShaderProxy * shaderProxy = mShaderListModel->Translate(index);
    mShaderEditorWidget->RequestOpen(shaderProxy);
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateEditorStyle()
{
    mShaderEditorWidget->OnSettingsChanged();
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::DispatchShaderEditorThroughProgramView(const QModelIndex& index)
{
    if (mProgramTreeModel->IsProgramIndex(index))
    {
    }
    else if (mProgramTreeModel->IsShaderIndex(index))
    {
        mShaderEditorWidget->show();
        mShaderEditorWidget->activateWindow();
        Pegasus::Shader::IShaderProxy * shaderProxy = mProgramTreeModel->TranslateShaderIndex(index);
        mShaderEditorWidget->RequestOpen(shaderProxy);
    }
}

//----------------------------------------------------------------------------------------

ShaderLibraryWidget::~ShaderLibraryWidget()
{
    delete mShaderManagerEventListener;
    delete mProgramTreeModel;
    delete mShaderListModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateUIForAppLoaded()
{
    InitializeInternalUserData();

    mProgramTreeModel->OnAppLoaded();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppLoaded();
    ui.ShaderTreeView->doItemsLayout();
    
    ActivateButtons(true);
}

void ShaderLibraryWidget::UpdateUIItemsLayout(void* targetShader)
{
    if (targetShader != nullptr)
    {
        mShaderEditorWidget->ShaderUIChanged(static_cast<Pegasus::Shader::IShaderProxy*>(targetShader));
    } 
    ui.ProgramTreeView->doItemsLayout();
    ui.ShaderTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::InitializeInternalUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy * shaderManager = appProxy->GetShaderManager();
            ED_ASSERTSTR(shaderManager != nullptr, "Failed retrieving shader manager");            
            for (int i = 0; i < shaderManager->GetShaderCount(); ++i)
            {
                Pegasus::Shader::IShaderProxy * shader = shaderManager->GetShader(i);
                ShaderUserData * newUserData = new ShaderUserData(shader);                   
                shader->SetUserData(newUserData);
            } 

            for (int i = 0; i < shaderManager->GetProgramCount(); ++i)
            {
                Pegasus::Shader::IProgramProxy * program = shaderManager->GetProgram(i);
                ProgramUserData * newUserData = new ProgramUserData(program);
                program->SetUserData(newUserData);
            } 
        }
    }
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UninitializeInternalUserData()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            Pegasus::Shader::IShaderManagerProxy * shaderManager = appProxy->GetShaderManager();
            ED_ASSERTSTR(shaderManager != nullptr, "Failed retrieving shader manager");            
            for (int i = 0; i < shaderManager->GetShaderCount(); ++i)
            {
                Pegasus::Shader::IShaderProxy * shader = shaderManager->GetShader(i);
                delete shader->GetUserData();
                shader->SetUserData(nullptr);
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

void ShaderLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    ActivateButtons(false);

    UninitializeInternalUserData();
}

