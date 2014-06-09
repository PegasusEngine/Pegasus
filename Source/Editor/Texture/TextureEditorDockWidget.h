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

#include "Viewport/ViewportType.h"

#include <QDockWidget>
#include "ui_TextureEditorDockWidget.h"

class ViewportWidget;


//! Dock widget for the texture editor
class TextureEditorDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent of the dock widget
    TextureEditorDockWidget(QWidget *parent = 0);

    //! Destructor
    ~TextureEditorDockWidget();

    //! Get the viewport widget associated with the dock widget
    //! \return Viewport widget associated with the dock widget
    inline ViewportWidget * GetViewportWidget() const { return mViewportWidget; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the graph has changed (typically to redraw the preview)
    void GraphChanged();

    //------------------------------------------------------------------------------------

public slots:

    //! Called when an application is successfully loaded
    void UpdateUIForAppLoaded();

    //! Called when an application is closed
    void UpdateUIForAppClosed();

    //! Called when the properties of the textures must be updated
    void UpdateTextureProperties();

    //------------------------------------------------------------------------------------
    
private:

    //! UI associated with the dock widget
    Ui::TextureEditorDockWidget ui;

    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;
};


#endif // EDITOR_TEXTUREEDITORDOCKWIDGET_H
