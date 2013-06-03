#ifndef VIEWPORTDOCKWIDGET_H
#define VIEWPORTDOCKWIDGET_H

#include <QDockWidget>

class ViewportWidget;


class ViewportDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    ViewportDockWidget(QWidget *parent = 0);
    ~ViewportDockWidget();

private:
    ViewportWidget * mViewportWidget;
};

#endif // VIEWPORTDOCKWIDGET_H
