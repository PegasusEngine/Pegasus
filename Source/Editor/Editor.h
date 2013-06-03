#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_Editor.h"

class ViewportDockWidget;
class TimelineDockWidget;

//----------------------------------------------------------------------------------------

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    Editor(QWidget *parent = 0);
    ~Editor();

private slots:
    void OpenViewportDockWidget();
    void OpenTimelineDockWidget();

private:
    Ui::EditorClass ui;

    // Dock widgets
    ViewportDockWidget * mViewportDockWidget;
    TimelineDockWidget * mTimelineDockWidget;
};

#endif // EDITOR_H
