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
:   QDockWidget(parent)
{
    ui.setupUi(this);

    // Set the dock widget parameters
    setWindowTitle(tr("Timeline"));
    setObjectName("TimelineDockWidget");
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    // Make connections between the UI elements and the child views
    connect(ui.addButton, SIGNAL(clicked()),
            ui.graphicsView, SLOT(AddLane()));
    connect(ui.graphicsView, SIGNAL(BeatUpdated(float)),
            this, SLOT(UpdateBeat(float)));

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

//----------------------------------------------------------------------------------------

void TimelineDockWidget::EnableAntialiasing(bool enable)
{
    ui.graphicsView->EnableAntialiasing(enable);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateBeat(float beat)
{
    const int intBeat = static_cast<int>(floorf(beat));
    const float fracBeat = beat - static_cast<float>(intBeat);
    const int subBeat = static_cast<int>(floorf(fracBeat * 8.0f));

    ui.beatLabel->setText(QString("Beat: %1:%2").arg(intBeat).arg(subBeat));
}
