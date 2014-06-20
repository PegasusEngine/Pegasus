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
class ShaderManagerEventListener;

//! Graphics Widget meant for shader navigation & management
class ShaderLibraryWidget : public QDockWidget
{
    Q_OBJECT

public:
    ShaderLibraryWidget(QWidget * parent, ShaderEditorWidget * editorWidget);
    virtual ~ShaderLibraryWidget();

public slots:
    //! slot triggered when app is loaded
    void UpdateUIForAppLoaded();

    //! updates the ui items laytout
    void UpdateUIItemsLayout();

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
    
    //! slot triggered when the editor style has changed, forces the text editor to resync styles
    void UpdateEditorStyle();

    //! slot triggered when there is a program linking event
    void OnProgramLinkingEvent(void * program, QString message, int eventType);

    //! gets the shader editor widget
    ShaderEditorWidget * GetShaderEditorWidget() { return mShaderEditorWidget; }

    //! gets the shader manager event listener
    ShaderManagerEventListener * GetShaderManagerEventListener() { return mShaderManagerEventListener;}

private:
    //! toggle the buttons on this widget on / off
    void ActivateButtons(bool activation);

    //! inject all user data inside shaders and program nodes 
    void InitializeInternalUserData();

    //! delete all user data inside shaders and program nodes
    void UninitializeInternalUserData();

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

    //! shader manager event listener
    ShaderManagerEventListener * mShaderManagerEventListener;

};

#endif
