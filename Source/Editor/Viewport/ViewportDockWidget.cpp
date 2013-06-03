#include "ViewportDockWidget.h"
#include "ViewportWidget.h"


ViewportDockWidget::ViewportDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
    // Set the dock widget parameters
    setWindowTitle(tr("Viewport"));
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create the viewport widget that will contain the renderer
    mViewportWidget = new ViewportWidget(this);
    setWidget(mViewportWidget);
}

//----------------------------------------------------------------------------------------

ViewportDockWidget::~ViewportDockWidget()
{
    delete mViewportWidget;
}
