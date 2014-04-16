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
#include "Pegasus/Shader/Shared/IShaderProxy.h"
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
    ui.setupUi(this);

    connect(ui.ShaderEditorButton, SIGNAL(clicked()),
            this, SLOT(DispatchShaderEditButton()));

    connect(ui.ProgramEditButton, SIGNAL(clicked()),
            this, SLOT(DispatchProgramEditButton()));

    connect(ui.ShaderTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchShaderEditorThroughShaderView(QModelIndex)));

    connect(ui.ProgramTreeView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(DispatchShaderEditorThroughProgramView(QModelIndex)));

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
    delete mProgramTreeModel;
    delete mShaderListModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateUIForAppLoaded()
{
    mProgramTreeModel->OnAppLoaded();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppLoaded();
    ui.ShaderTreeView->doItemsLayout();
    
    ActivateButtons(true);
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();

    ActivateButtons(false);
}

