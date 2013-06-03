#ifndef TIMELINEDOCKWIDGET_H
#define TIMELINEDOCKWIDGET_H

#include <QDockWidget>
#include "ui_TimelineDockWidget.h"

class TimelineDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    TimelineDockWidget(QWidget *parent = 0);
    ~TimelineDockWidget();

private:
    Ui::TimelineDockWidget ui;
};

#endif // TIMELINEDOCKWIDGET_H
