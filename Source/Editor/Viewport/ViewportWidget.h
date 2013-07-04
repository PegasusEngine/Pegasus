/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#ifndef EDITOR_VIEWPORTWIDGET_H
#define EDITOR_VIEWPORTWIDGET_H

#include <QWidget>


//! Widget containing one application viewport
class ViewportWidget : public QWidget
{
    Q_OBJECT

public:

    ViewportWidget(QWidget *parent);
    ~ViewportWidget();
};


#endif  // EDITOR_VIEWPORTWIDGET_H
