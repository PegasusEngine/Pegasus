/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDockWidget.cpp
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Dock widget containing the timeline graphics view

#include "Timeline/TimelineDockWidget.h"
#include "Timeline/TimelineUndoCommands.h"

#include "Application/Application.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include <QMessagebox>

#include <QListWidgetItem>
#include <QUndoStack>


TimelineDockWidget::TimelineDockWidget(QWidget *parent, Editor* editor)
    :   
    PegasusDockWidget(parent, editor),
    mSnapNumTicks(1),
    mEnableUndo(true)
{
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetupUi()
{
    // Set the initial UI state
    ui.setupUi(this);
    ui.propertyGridWidget->SetMessenger(this);

    mUndoStack = new QUndoStack(this);

    UpdateUIForAppClosed();

	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    ui.graphicsView->SetUndoStack(GetCurrentUndoStack());
    ui.graphicsView->installEventFilter(this);

    // Make connections between the UI elements and the child views
    connect(ui.saveButton, SIGNAL(clicked()),
            this, SLOT(SaveTimeline()));
    connect(ui.addButton, SIGNAL(clicked()),
            ui.graphicsView, SLOT(AddLane()));
    connect(ui.playButton, SIGNAL(toggled(bool)),
            ui.graphicsView, SLOT(OnPlayModeToggled(bool)));
    connect(ui.bpmSpin, SIGNAL(valueChanged(double)),
            this, SLOT(OnBeatsPerMinuteChanged(double)));
    connect(ui.snapCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnSnapModeChanged(int)));
    connect(ui.graphicsView, SIGNAL(BeatUpdated(float)),
            this, SLOT(SetCurrentBeat(float)));
    connect(ui.graphicsView, SIGNAL(BlockSelected(Pegasus::Timeline::IBlockProxy*)),
            this, SLOT(OnBlockSelected(Pegasus::Timeline::IBlockProxy*)));
    connect(ui.graphicsView, SIGNAL(MultiBlocksSelected()),
            this, SLOT(OnMultiBlocksSelected()));
    connect(ui.graphicsView, SIGNAL(BlocksDeselected()),
            this, SLOT(OnBlocksDeselected()));

    connect(ui.playButton, SIGNAL(toggled(bool)),
            this, SIGNAL(PlayModeToggled(bool)));
    connect(ui.graphicsView, SIGNAL(BlockMoved()),
            this, SIGNAL(BlockMoved()));
    connect(ui.graphicsView, SIGNAL(BlockSelected(Pegasus::Timeline::IBlockProxy*)),
            this, SIGNAL(BlockSelected(Pegasus::Timeline::IBlockProxy*)));
    connect(ui.graphicsView, SIGNAL(BlocksDeselected()),
            this, SIGNAL(BlocksDeselected()));
    connect(ui.graphicsView, SIGNAL(MultiBlocksSelected()),
            this, SIGNAL(MultiBlocksSelected()));
    connect(ui.graphicsView, SIGNAL(BlockDoubleClicked(Pegasus::Timeline::IBlockProxy*)),
            this, SIGNAL(BlockDoubleClicked(Pegasus::Timeline::IBlockProxy*)));
}

//----------------------------------------------------------------------------------------

TimelineDockWidget::~TimelineDockWidget()
{
}

//----------------------------------------------------------------------------------------

bool TimelineDockWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (ui.graphicsView == obj)
    {
        if (event->type() == QEvent::FocusIn)
        {
            emit(OnFocus(this));
        }
        if (event->type() == QEvent::FocusOut)
        {
            emit(OutFocus(this));
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::EnableAntialiasing(bool enable)
{
    ui.graphicsView->EnableAntialiasing(enable);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetBeatsPerMinute(double bpm)
{
    Application * const application = GetEditor()->GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        Pegasus::Timeline::ITimelineManagerProxy * const timeline = application->GetTimelineProxy();
        if (timeline != nullptr)
        {
            mEnableUndo = false;

            // Apply the new tempo to the timeline
            timeline->GetCurrentTimeline()->SetBeatsPerMinute(static_cast<float>(bpm));

            // Update the tempo field
            ui.bpmSpin->setValue(bpm);

            // Update the timeline view from the new tempo
            if (timeline->GetCurrentTimeline()->GetCurrentBeat() >= 0.0f)
            {
                UpdateUIFromBeat(timeline->GetCurrentTimeline()->GetCurrentBeat());
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

void TimelineDockWidget::OnSaveFocusedObject()
{
    SaveTimeline();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SaveTimeline()
{
    Application * const application = GetEditor()->GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        Pegasus::Timeline::ITimelineManagerProxy * const timeline = application->GetTimelineProxy();
        
        if (timeline->GetCurrentTimeline() != nullptr)
        {
            AssetIOMessageController::Message msg;
            msg.SetMessageType(AssetIOMessageController::Message::SAVE_ASSET);
            msg.SetObject(timeline->GetCurrentTimeline());
            SendAssetIoMessage(msg);
        }
    }
    
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnReceiveAssetIoMessage(AssetIOMessageController::Message::IoResponseMessage msg)
{
    switch(msg)
    {
    case AssetIOMessageController::Message::IO_SAVE_ERROR:
        {
            QMessageBox::warning(
                this, "Error saving timeline.",
                "Timeline could not be saved. Make sure asset is not read only and exists on disc."
            );
        }
        break;
    default:
        break;
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* applicationProxy)
{
    mEnableUndo = false;

    ui.addButton->setEnabled(true);
    ui.removeButton->setEnabled(true);
    ui.deleteButton->setEnabled(true);

    ui.playButton->setEnabled(true);
    ui.playButton->setChecked(false);
    ui.graphicsView->OnPlayModeToggled(false);

    if (applicationProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineManagerProxy * const timeline = applicationProxy->GetTimelineManagerProxy();
        timeline->GetCurrentTimeline()->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);
        ui.bpmSpin->setValue(static_cast<double>(timeline->GetCurrentTimeline()->GetBeatsPerMinute()));
    }
    else
    {
        ui.bpmSpin->setValue(138.0);
    }
    ui.bpmSpin->setEnabled(true);

    ui.snapCombo->setEnabled(true);
    ui.snapCombo->setCurrentIndex(0);
    mSnapNumTicks = 1;
    UpdateUIFromBeat(0.0f);
    ui.graphicsView->setEnabled(true);

    ui.propertyGridWidget->SetApplicationProxy(applicationProxy);

    // Update the content of the timeline graphics view from the timeline of the app
    ui.graphicsView->RefreshFromTimeline();

    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnUIForAppClosed()
{
    ui.addButton->setEnabled(false);
    ui.removeButton->setEnabled(false);
    ui.deleteButton->setEnabled(false);

    ui.playButton->setEnabled(false);
    ui.playButton->setChecked(false);

    ui.propertyGridWidget->SetApplicationProxy(nullptr);

    // Clear the content of the timeline graphics view and set the default timeline
    ui.graphicsView->Initialize();
    ui.graphicsView->OnPlayModeToggled(false);

    ui.bpmSpin->setEnabled(false);
    ui.bpmSpin->setValue(138.0);
    ui.snapCombo->setEnabled(false);
    ui.snapCombo->setCurrentIndex(0);
    mSnapNumTicks = 1;
    UpdateUIFromBeat(0.0f);
    ui.graphicsView->setEnabled(false);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBeatsPerMinuteChanged(double bpm)
{
    if (mEnableUndo)
    {
        Application * const application = GetEditor()->GetApplicationManager().GetApplication();
        if (application != nullptr)
        {
            Pegasus::Timeline::ITimelineManagerProxy * const timeline = application->GetTimelineProxy();

            // Create the undo command
            TimelineSetBPMUndoCommand * undoCommand = new TimelineSetBPMUndoCommand(static_cast<double>(timeline->GetCurrentTimeline()->GetBeatsPerMinute()),
                                                                                    bpm);
            mUndoStack->push(undoCommand);
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnSnapModeChanged(int mode)
{
    if (mode == 0)
    {
        mSnapNumTicks = 1;  // No snapping, use the smallest unit, the tick
        ED_LOG("Number of ticks for timeline block snap: 1");
        return;
    }
    
    Application * const application = GetEditor()->GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        Pegasus::Timeline::ITimelineManagerProxy * const timeline = application->GetTimelineProxy();
        if (timeline != nullptr)
        {
            unsigned int numTicksPerBeat = timeline->GetCurrentTimeline()->GetNumTicksPerBeat();

            switch (mode)
            {
                case 1:     mSnapNumTicks = numTicksPerBeat / 16;   break;
                case 2:     mSnapNumTicks = numTicksPerBeat / 8;    break;
                case 3:     mSnapNumTicks = numTicksPerBeat / 4;    break;
                case 4:     mSnapNumTicks = numTicksPerBeat / 2;    break;
                case 5:     mSnapNumTicks = numTicksPerBeat;        break;
                case 6:     mSnapNumTicks = numTicksPerBeat * 2;    break;
                case 7:     mSnapNumTicks = numTicksPerBeat * 4;    break;
                case 8:     mSnapNumTicks = numTicksPerBeat * 8;    break;

                default:
                    ED_FAILSTR("Invalid snap mode (%d), it should be <= 8", mode);
                    mSnapNumTicks = 1;
                    break;
            }

            ED_LOG("Number of ticks for timeline block snap: %u", mSnapNumTicks);
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

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlockSelected(Pegasus::Timeline::IBlockProxy * blockProxy)
{
    ui.propertyGridWidget->SetCurrentProxy(blockProxy->GetPropertyGridProxy());
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnMultiBlocksSelected()
{
    ui.propertyGridWidget->SetCurrentProxy(nullptr);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlocksDeselected()
{
    ui.propertyGridWidget->SetCurrentProxy(nullptr);
}
