/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportDockWidget.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the application viewports

#ifndef EDITOR_VIEWPORTDOCKWIDGET_H
#define EDITOR_VIEWPORTDOCKWIDGET_H

#include "Viewport/ViewportType.h"

#include <QDockWidget>

class ViewportWidget;

class QMenuBar;


//! Dock widget containing the viewports
class ViewportDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param viewportType Type of the viewport (VIEWPORTTYPE_xxx constant)
    //! \param parent Parent of the dock widget
    ViewportDockWidget(ViewportType viewportType, QWidget * parent = 0);

    //! Destructor
    virtual ~ViewportDockWidget();

    //! Get the viewport widget associated with the dock widget
    //! \return Viewport widget associated with the dock widget
    //! \todo Handle multiple viewports
    inline ViewportWidget * GetViewportWidget() const { return mViewportWidget; }

private:

    //! Create the menu of the dock widget
    //! \param mainWidget Main widget of the dock, covering the entire child area
    //! \return Menu bar object to attach to the widget's layout
    QMenuBar * CreateMenu(QWidget * mainWidget);


    //! Viewport widget embedded in the dock widget
    ViewportWidget * mViewportWidget;
};


#endif  // EDITOR_VIEWPORTDOCKWIDGET_H
