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

ShaderLibraryWidget::ShaderLibraryWidget(QWidget * parent)
: QDockWidget(parent)
{
    mProgramTreeModel = new ProgramTreeModel(this);
    mShaderListModel = new ShaderListModel(this);
    ui.setupUi(this);

    //disable program buttons for now
    ui.ProgramAddButton->setDisabled(true);
    ui.ProgramRemoveButton->setDisabled(true);
    ui.ProgramEditButton->setDisabled(true);
    ui.ShaderAddButton->setDisabled(true);
    ui.ShaderRemoveButton->setDisabled(true);

    ui.ProgramTreeView->setModel(mProgramTreeModel);
    ui.ShaderTreeView->setModel(mShaderListModel);
}

//----------------------------------------------------------------------------------------

ShaderLibraryWidget::~ShaderLibraryWidget()
{
    delete mProgramTreeModel;
}

// SLOTS
//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateUIForAppLoaded()
{
    mProgramTreeModel->OnAppLoaded();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppLoaded();
    ui.ShaderTreeView->doItemsLayout();
}

//----------------------------------------------------------------------------------------

void ShaderLibraryWidget::UpdateUIForAppFinished()
{    
    mProgramTreeModel->OnAppDestroyed();
    ui.ProgramTreeView->doItemsLayout();

    mShaderListModel->OnAppDestroyed();
    ui.ShaderTreeView->doItemsLayout();
}

