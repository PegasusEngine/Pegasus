#include "TimelineDockWidget.h"


TimelineDockWidget::TimelineDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
    ui.setupUi(this);

    // Set the dock widget parameters
    setWindowTitle(tr("Timeline"));
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
}

//----------------------------------------------------------------------------------------

TimelineDockWidget::~TimelineDockWidget()
{

}
