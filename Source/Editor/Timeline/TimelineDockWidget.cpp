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
#include "Timeline/TimelineUndoCommands.h"

#include "Application/Application.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"


TimelineDockWidget::TimelineDockWidget(QWidget *parent)
:   QDockWidget(parent),
    mEnableUndo(true)
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
    connect(ui.playButton, SIGNAL(toggled(bool)),
            ui.graphicsView, SLOT(OnPlayModeToggled(bool)));
    connect(ui.bpmSpin, SIGNAL(valueChanged(double)),
            this, SLOT(OnSetBeatsPerMinute(double)));
    connect(ui.graphicsView, SIGNAL(BeatUpdated(float)),
            this, SLOT(SetCurrentBeat(float)));
    connect(ui.graphicsView, SIGNAL(BlockMoved()),
            this, SIGNAL(BlockMoved()));
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

void TimelineDockWidget::SetBeatsPerMinute(double bpm)
{
    Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * const timeline = application->GetTimelineProxy();
        if (timeline != nullptr)
        {
            mEnableUndo = false;

            // Apply the new tempo to the timeline
            timeline->SetBeatsPerMinute(static_cast<float>(bpm));

            // Update the tempo field
            ui.bpmSpin->setValue(bpm);

            // Update the timeline view from the new tempo
            if (timeline->GetCurrentBeat() >= 0.0f)
            {
                UpdateUIFromBeat(timeline->GetCurrentBeat());
            }

            mEnableUndo = true;
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateUIFromBeat(float beat)
{
    if (beat < 0.0f)
    {
        ED_FAILSTR("Invalid beat (%f) given to the timeline widget", beat);
        beat = 0.0f;
    }

    // Update the beat label
    const unsigned int intBeat = static_cast<unsigned int>(floorf(beat));
    const float fracBeat = beat - static_cast<float>(intBeat);
    const unsigned int subBeat = static_cast<unsigned int>(floorf(fracBeat * 4.0f));
    const float subFracBeat = fracBeat - static_cast<float>(subBeat) * (1.0f / 4.0f);
    const unsigned int subSubBeat = static_cast<unsigned int>(floorf(subFracBeat * (4.0f * 4.0f)));
    SetBeatLabel(intBeat, subBeat, subSubBeat);

    // Update the time label
    const double timeMinutes = static_cast<double>(beat) / ui.bpmSpin->value();
    const unsigned int intMinutes = static_cast<unsigned int>(floor(timeMinutes));
    const double fracTimeSeconds = (timeMinutes - static_cast<double>(intMinutes)) * 60.0;
    const unsigned int intSeconds = static_cast<unsigned int>(floor(fracTimeSeconds));
    const double fracTimeMilliseconds = (fracTimeSeconds - static_cast<double>(intSeconds)) * 1000.0;
    const unsigned int intMilliseconds = static_cast<unsigned int>(floor(fracTimeMilliseconds));
    SetTimeLabel(intMinutes, intSeconds, intMilliseconds);

    // Place the cursor in the right position
    ui.graphicsView->SetCursorFromBeat(beat);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateUIForAppLoaded()
{
    mEnableUndo = false;

    ui.addButton->setEnabled(true);
    ui.removeButton->setEnabled(true);
    ui.deleteButton->setEnabled(true);

    ui.playButton->setEnabled(true);
    ui.playButton->setChecked(false);
    ui.graphicsView->OnPlayModeToggled(false);

    Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * const timeline = application->GetTimelineProxy();
        ui.bpmSpin->setValue(static_cast<double>(timeline->GetBeatsPerMinute()));
    }
    else
    {
        ui.bpmSpin->setValue(120.0);
    }
    ui.bpmSpin->setEnabled(true);

    ui.snapCombo->setEnabled(true);
    UpdateUIFromBeat(0.0f);
    ui.graphicsView->setEnabled(true);

    // Update the content of the timeline graphics view from the timeline of the app
    ui.graphicsView->RefreshFromTimeline();

    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::UpdateUIForAppClosed()
{
    ui.addButton->setEnabled(false);
    ui.removeButton->setEnabled(false);
    ui.deleteButton->setEnabled(false);

    ui.playButton->setEnabled(false);
    ui.playButton->setChecked(false);
    ui.graphicsView->OnPlayModeToggled(false);

    ui.bpmSpin->setEnabled(false);
    ui.snapCombo->setEnabled(false);
    UpdateUIFromBeat(0.0f);
    ui.graphicsView->setEnabled(false);

    // Clear the content of the timeline graphics view
    if (Editor::GetInstance().IsApplicationManagerAvailable())
    {
        ui.graphicsView->RefreshFromTimeline();
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnSetBeatsPerMinute(double bpm)
{
    if (mEnableUndo)
    {
        Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
        if (application != nullptr)
        {
            Pegasus::Timeline::ITimelineProxy * const timeline = application->GetTimelineProxy();

            // Create the undo command
            TimelineSetBPMUndoCommand * undoCommand = new TimelineSetBPMUndoCommand(static_cast<double>(timeline->GetBeatsPerMinute()),
                                                                                    bpm);
        
            // Push the undo command, redo() is executed and the timeline updated
            Editor::GetInstance().PushUndoCommand(undoCommand);
        }
    }
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
    ui.beatLabel->setText(QString("%1:%2.%3").arg(beat).arg(subBeat).arg(subSubBeat));
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetTimeLabel(unsigned int minutes, unsigned int seconds, unsigned int milliseconds)
{
    ui.timeLabel->setText(QString("%1:%2:%3").arg(minutes).arg(seconds, 2, 10, QChar('0')).arg(milliseconds, 3, 10, QChar('0')));
}
