/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDockWidget.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the timeline graphics view

#ifndef EDITOR_TIMELINEDOCKWIDGET_H
#define EDITOR_TIMELINEDOCKWIDGET_H

#include <QDockWidget>
#include "ui_TimelineDockWidget.h"


//! Dock widget containing the timeline graphics view
class TimelineDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    TimelineDockWidget(QWidget *parent = 0);
    ~TimelineDockWidget();

private:

    Ui::TimelineDockWidget ui;
};


#endif  // EDITOR_TIMELINEDOCKWIDGET_H
