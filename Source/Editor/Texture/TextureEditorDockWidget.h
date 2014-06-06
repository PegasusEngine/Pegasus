/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEditorDockWidget.h
//! \author	Kevin Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the texture editor

#ifndef EDITOR_TEXTUREEDITORDOCKWIDGET_H
#define EDITOR_TEXTUREEDITORDOCKWIDGET_H

#include <QDockWidget>
#include "ui_TextureEditorDockWidget.h"


//! Dock widget for the texture editor
class TextureEditorDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    TextureEditorDockWidget(QWidget *parent = 0);
    ~TextureEditorDockWidget();

    //------------------------------------------------------------------------------------
    
private:
    Ui::TextureEditorDockWidget ui;
};


#endif // EDITOR_TEXTUREEDITORDOCKWIDGET_H
