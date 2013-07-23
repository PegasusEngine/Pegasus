/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the timeline graphics view

#include "Timeline/TimelineDockWidget.h"


TimelineDockWidget::TimelineDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
    ui.setupUi(this);

    // Set the dock widget parameters
    setWindowTitle(tr("Timeline"));
    setObjectName("TimelineDockWidget");
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    //! \todo Temporary! Just to have proper scene rect
    ui.graphicsView->AddLane();
    ui.graphicsView->AddLane();
    ui.graphicsView->AddLane();
    ui.graphicsView->AddLane();
}

//----------------------------------------------------------------------------------------

TimelineDockWidget::~TimelineDockWidget()
{

}
