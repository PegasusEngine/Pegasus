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
    // Set the initial UI state
    ui.setupUi(this);
    UpdateUIForAppClosed();

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
    connect(ui.playButton, SIGNAL(toggled(bool)),
            this, SIGNAL(PlayModeToggled(bool)));
    connect(ui.graphicsView, SIGNAL(BeatUpdated(float)),
            this, SLOT(SetCurrentBeat(float)));

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

void TimelineDockWidget::UpdateUIFromBeat(float beat)
{
    if (beat < 0.0f)
    {
        ED_FAILSTR("Invalid beat (%f) given to the timeline widget", beat);
        beat = 0.0f;
    }

    const unsigned int intBeat = static_cast<unsigned int>(floorf(beat));
    const float fracBeat = beat - static_cast<float>(intBeat);
    const unsigned int subBeat = static_cast<unsigned int>(floorf(fracBeat * 4.0f));
    const float subFracBeat = fracBeat - static_cast<float>(subBeat) * (1.0f / 4.0f);
    const unsigned int subSubBeat = static_cast<unsigned int>(floorf(subFracBeat * (4.0f * 4.0f)));

    SetBeatLabel(intBeat, subBeat, subSubBeat);
    ui.graphicsView->SetCursorFromBeat(beat);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateUIForAppLoaded()
{
    ui.addButton->setEnabled(true);
    ui.removeButton->setEnabled(true);
    ui.deleteButton->setEnabled(true);
    ui.playButton->setEnabled(true);
    ui.playButton->setChecked(false);
    UpdateUIFromBeat(0.0f);

    //! \todo Enable the timeline graphics view input
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateUIForAppClosed()
{
    ui.addButton->setEnabled(false);
    ui.removeButton->setEnabled(false);
    ui.deleteButton->setEnabled(false);
    ui.playButton->setEnabled(false);
    ui.playButton->setChecked(false);
    UpdateUIFromBeat(0.0f);

    //! \todo Disable the timeline graphics view input
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetCurrentBeat(float beat)
{
    emit BeatUpdated(beat);
    UpdateUIFromBeat(beat);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetBeatLabel(unsigned int beat, unsigned int subBeat, unsigned int subSubBeat)
{
    ui.beatLabel->setText(QString("Beat: %1:%2.%3").arg(beat).arg(subBeat).arg(subSubBeat));
}
