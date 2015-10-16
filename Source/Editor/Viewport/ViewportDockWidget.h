/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportDockWidget.h
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the application viewports

#ifndef EDITOR_VIEWPORTDOCKWIDGET_H
#define EDITOR_VIEWPORTDOCKWIDGET_H

#include "Widgets/PegasusDockWidget.h"

#include <QDockWidget>

class ViewportWidget;
class Editor;
class QMenuBar;


//! Dock widget containing the viewports
class ViewportDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent editor
    //! \param parent Parent of the dock widget
    //! \param title of this widget
    //! \param object name of this widget
    ViewportDockWidget(
        Editor* editor, 
        QWidget * parent, 
        const char* title, 
        const char* objName);

    //! Destructor
    virtual ~ViewportDockWidget();

    //! Get the viewport widget associated with the dock widget
    //! \return Viewport widget associated with the dock widget
    inline ViewportWidget * GetViewportWidget() const { return mViewportWidget; }

    //! Callback fired when the UI needs to be set.
    virtual void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const { return mObjName; }

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return mTitle; }

    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();

private:

    //! Create the menu of the dock widget
    //! \param mainWidget Main widget of the dock, covering the entire child area
    //! \return Menu bar object to attach to the widget's layout
    QMenuBar * CreateMenu(QWidget * mainWidget);


    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;

    //! Data for pegasus display functions
    const char* mTitle;
    const char* mObjName;
};


#endif  // EDITOR_VIEWPORTDOCKWIDGET_H
