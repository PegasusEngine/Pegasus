/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureEditorDockWidget.h
//! \author	Karolyn Boulanger
//! \date	05th June 2014
//! \brief	Dock widget for the texture editor

#ifndef EDITOR_TEXTUREEDITORDOCKWIDGET_H
#define EDITOR_TEXTUREEDITORDOCKWIDGET_H

#include <QDockWidget>
#include "Widgets/PegasusDockWidget.h"
#include "ui_TextureEditorDockWidget.h"

class ViewportWidget;
class QMdiSubWindow;
class Editor;

namespace Pegasus {
    namespace Texture {
        class ITextureNodeProxy;
    }

    namespace App {
        class IApplicationProxy;
    }
}


//! Dock widget for the texture editor
class TextureEditorDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent of the dock widget
    TextureEditorDockWidget(QWidget *parent, Editor* editor);

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const { return "TextureEditorDockWidget"; } 

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Texture Editor"; }

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

    //! Called to open a new tab for a given texture
    //! \param textureProxy Proxy of the texture to attach to the tab
    void OpenTabForTexture(Pegasus::Texture::ITextureNodeProxy * textureProxy);

    //------------------------------------------------------------------------------------
    
private slots:
        
    //! Called when a different tab is selected (or when the tab is created)
    //! \param subWindow Subwindow corresponding to the selected tab
    void TabSelected(QMdiSubWindow * subWindow);

    //------------------------------------------------------------------------------------

protected:
    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

    //------------------------------------------------------------------------------------
    
private:

    //! Called when the properties of the textures must be updated
    //! \param subWindow MDI subwindow that is currently active
    void UpdateTextureProperties(QMdiSubWindow * subWindow);


    //! UI associated with the dock widget
    Ui::TextureEditorDockWidget ui;

    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;
};


#endif // EDITOR_TEXTUREEDITORDOCKWIDGET_H
