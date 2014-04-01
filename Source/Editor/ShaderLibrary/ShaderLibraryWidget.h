/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderLibraryWidget.h
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Graphics widget representing the shader library form.

#ifndef EDITOR_SHADERLIBRARYWIDGET_H
#define EDITOR_SHADERLIBRARYWIDGET_H

#include <QDockWidget>
#include <QAction>
#include "ui_ShaderLibraryWidget.h"

class QItemSelectionModel;
class ShaderEditorWidget;
class ProgramTreeModel;
class ShaderListModel;

//! Graphics Widget meant for shader navigation & management
class ShaderLibraryWidget : public QDockWidget
{
    Q_OBJECT

public:
    ShaderLibraryWidget(QWidget * parent = 0);
    virtual ~ShaderLibraryWidget();

public slots:
    //! slot triggered when app is loaded
    void UpdateUIForAppLoaded();

    //! slot triggered when app is unloaded
    void UpdateUIForAppFinished();

    //! slot triggered when shader editor is dispatched
    void DispatchShaderEditButton();

    //! slot triggered when a program editor is to be dispatched
    void DispatchProgramEditButton();

    //! slot triggered when something has been selected through shader tree view
    void DispatchShaderEditorThroughShaderView(const QModelIndex& index);

    //! slot triggered when something has been selected through program tree view
    void DispatchShaderEditorThroughProgramView(const QModelIndex& index);

    
    

private:
    //! toggle the buttons on this widget on / off
    void ActivateButtons(bool activation);

    //! ui components
    Ui::ShaderLibraryWidget ui;

    //! reference to the program tree data model
    ProgramTreeModel * mProgramTreeModel;

    //! reference to the program tree selection model (used to determine what is selected)
    QItemSelectionModel * mProgramSelectionModel;

    //! reference to the shader list data model
    ShaderListModel * mShaderListModel;

    //! reference to the shader list selection model (used to determine what shader is selected)
    QItemSelectionModel * mShaderListSelectionModel;
    
    //! reference to the shader text editor widget
    ShaderEditorWidget * mShaderEditorWidget;

};

#endif
