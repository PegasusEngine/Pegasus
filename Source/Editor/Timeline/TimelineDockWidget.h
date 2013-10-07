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
    virtual ~TimelineDockWidget();

    //! Set the antialiasing mode of the timeline
    //! \param enable True to enable antialiasing
    void EnableAntialiasing(bool enable);


private slots:

    //! Called when the current beat has been updated
    //! \param beat Current beat, can have fractional part
    void UpdateBeat(float beat);


private:

    Ui::TimelineDockWidget ui;
};


#endif  // EDITOR_TIMELINEDOCKWIDGET_H
