#include "Editor.h"
#include "Viewport/ViewportDockWidget.h"
#include "Timeline/TimelineDockWidget.h"


Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Create the dock widgets
    mViewportDockWidget = new ViewportDockWidget(this);
    mTimelineDockWidget = new TimelineDockWidget(this);

    // Connect the Windows menu items to the dock widgets to make them visible
    connect(ui.action_Viewport, SIGNAL(triggered()), this, SLOT(OpenViewportDockWidget()));
    connect(ui.action_Timeline, SIGNAL(triggered()), this, SLOT(OpenTimelineDockWidget()));
}

//----------------------------------------------------------------------------------------

Editor::~Editor()
{
    delete mViewportDockWidget;
    delete mTimelineDockWidget;
}

//----------------------------------------------------------------------------------------

void Editor::OpenViewportDockWidget()
{
    mViewportDockWidget->show();
}

//----------------------------------------------------------------------------------------

void Editor::OpenTimelineDockWidget()
{
    mTimelineDockWidget->show();
}