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
#include "Timeline/TimelineBlockGraphicsItem.h"

#include "Application/Application.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/PegasusAssetTypes.h"
#include <QMessagebox>
#include <QFileDialog>

#include <QListWidgetItem>
#include <QUndoStack>
#include <QMenu>


TimelineDockWidget::TimelineDockWidget(QWidget *parent, Editor* editor)
    :   
    PegasusDockWidget(parent, editor),
    mSnapNumTicks(1),
    mEnableUndo(true),
    mApplication(nullptr),
    mIsCursorQueued(false),
    mObserver(this)
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
    connect(ui.graphicsView, SIGNAL(BlockSelected(unsigned)),
            this, SLOT(OnBlockSelected(unsigned)));
    connect(ui.graphicsView, SIGNAL(MultiBlocksSelected()),
            this, SLOT(OnMultiBlocksSelected()));
    connect(ui.graphicsView, SIGNAL(BlocksDeselected()),
            this, SLOT(OnBlocksDeselected()));
    connect(ui.graphicsView, SIGNAL(RequestChangeScript(QGraphicsObject*,unsigned)),
            this, SLOT(RequestChangeScript(QGraphicsObject*,unsigned)));
    connect(ui.graphicsView, SIGNAL(RequestRemoveScript(QGraphicsObject*,unsigned)),
            this, SLOT(RequestRemoveScript(QGraphicsObject*,unsigned)));
    connect(ui.graphicsView, SIGNAL(RequestBlockMove(QGraphicsObject*, QPointF)),
            this, SLOT(RequestMoveBlock(QGraphicsObject*, QPointF)));
    connect(ui.graphicsView, SIGNAL(BlockDoubleClicked(QString)),
            this, SLOT(OnBlockDoubleClicked(QString)));

    connect(ui.playButton, SIGNAL(toggled(bool)),
            this, SIGNAL(PlayModeToggled(bool)));

    connect(ui.propertyGridWidget, SIGNAL(OnPropertyUpdated(QtProperty*)),
            this, SLOT(OnPropertyUpdated(QtProperty*)));

    connect(ui.loadMasterScriptButton, SIGNAL(clicked()),
            this, SLOT(RequestMasterTimelineScriptLoad()));

    mMasterScriptMenu = new QMenu();
    mEditMasterScriptButton = mMasterScriptMenu->addAction(tr("Edit master script."));
    mRemoveMasterScriptButton = mMasterScriptMenu->addAction(tr("Remove master script."));
    ui.removeScriptButton->setMenu(mMasterScriptMenu);
    ui.removeScriptButton->setPopupMode(QToolButton::InstantPopup);

    connect(mEditMasterScriptButton, SIGNAL(triggered()),
            this, SLOT(EditMasterScript()));

    connect(mRemoveMasterScriptButton, SIGNAL(triggered()),
            this, SLOT(RemoveMasterScript()));
}

//----------------------------------------------------------------------------------------

TimelineDockWidget::~TimelineDockWidget()
{
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnPropertyUpdated(QtProperty* property)
{
    if (property->propertyName() == "Name" ||
        property->propertyName() == "Color"||
        property->propertyName() == "Beat"||
        property->propertyName() == "Duration")
    {
        ui.graphicsView->FlushVisualProperties();
        ui.graphicsView->RedrawInternalBlocks();
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnRepaintTimeline()
{
    ui.graphicsView->RedrawInternalBlocks();
}

//----------------------------------------------------------------------------------------

QString TimelineDockWidget::AskForTimelineScript()
{
    QString rootFolder = mApplication->GetAssetsRoot();
    QDir qd(rootFolder);
    QString fileExtension = tr("%1 Shader (*.%2)").arg(tr(Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeName), tr(Pegasus::ASSET_TYPE_BLOCKSCRIPT.mExtension));
    QString requestedFile = QFileDialog::getOpenFileName(this, tr("Open shader."), rootFolder, fileExtension);
    QString filePath = qd.path();
    if (requestedFile.startsWith(filePath))
    {
        QString path = requestedFile.right(requestedFile.size() - filePath.size() - 1);
        return path;
    }
    else if (requestedFile.length() > 0)
    { 
        QMessageBox::warning(this, tr("Can't load asset."), tr("Asset must be loaded into the root directory only of the project. ")+rootFolder, QMessageBox::Ok);
        return tr("");
    }

    return tr("");
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestChangeScript(QGraphicsObject* sender, unsigned blockGuid)
{
    if (mApplication != nullptr)
    {
        QString requestedScript = AskForTimelineScript();
        if (requestedScript != "")
        {
            TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::SET_BLOCKSCRIPT);
            msg.SetBlockGuid(blockGuid);
            msg.SetString(requestedScript);
            SendTimelineIoMessage(msg);
            static_cast<TimelineBlockGraphicsItem*>(sender)->UIUpdateBlockScript(requestedScript);
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestRemoveScript(QGraphicsObject* sender, unsigned blockGuid)
{
    if (mApplication != nullptr)
    {
        TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::CLEAR_BLOCKSCRIPT);
        msg.SetBlockGuid(blockGuid);
        SendTimelineIoMessage(msg);
        static_cast<TimelineBlockGraphicsItem*>(sender)->UIClearBlockScript();
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestMoveBlock(QGraphicsObject* sender, QPointF amount)
{
    if (mApplication != nullptr && mTimelineHandle.IsValid())
    {
        //ask the render thread runtime the best available position for the candidate beat & lane.
        TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::BLOCK_OPERATION);
        TimelineBlockGraphicsItem* graphicsItem = static_cast<TimelineBlockGraphicsItem*>(sender);
        const ShadowBlockState& blockState = graphicsItem->GetBlockProxy();
        msg.SetBlockOp(TimelineIOMessageController::ASK_POSITION);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetBlockGuid(blockState.GetGuid());
        msg.SetLaneId(graphicsItem->GetLaneFromY(amount.y()));
        msg.SetObserver(&mObserver);
        msg.SetArg(QVariant(graphicsItem->GetBeatFromX(amount.x())));
        msg.SetMouseClickId(TimelineBlockGraphicsItem::sMouseClickID);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnFocusBlock(unsigned blockGuid)
{
    setFocus();
    ui.graphicsView->OnFocusBlock(blockGuid);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestMasterTimelineScriptLoad()
{
    if (mApplication != nullptr)
    {
        QString requestedScript = AskForTimelineScript();
        if (requestedScript != "")
        {
            TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::SET_MASTER_BLOCKSCRIPT);
            msg.SetString(requestedScript);
            SendTimelineIoMessage(msg);
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RemoveMasterScript()
{
    if (mApplication != nullptr)
    {
        TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::CLEAR_MASTER_BLOCKSCRIPT);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::EditMasterScript()
{
    if (mApplication != nullptr && mLoadedScript != "")
    {
        AssetIOMessageController::Message msg(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(mLoadedScript);
        SendAssetIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

const Pegasus::PegasusAssetTypeDesc*const* TimelineDockWidget::GetTargetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* sAssetTypes[] =  { &Pegasus::ASSET_TYPE_TIMELINE, nullptr };
    return sAssetTypes;
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
    if (mTimelineHandle.IsValid())
    {
        mEnableUndo = false;

        // Update the tempo field
        ui.bpmSpin->setValue(bpm);

        // Apply the new tempo to the timeline
        QVariant bpmArg(bpm);
        TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::SET_PARAMETER);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetParameterName((unsigned)ShadowTimelineState::PROP_BEATS_PER_MIN);
        msg.SetArg(bpmArg);
        msg.SetTarget(TimelineIOMessageController::TIMELINE_OBJECT);
        msg.SetObserver(&mObserver);
        SendTimelineIoMessage(msg);

        // Update the timeline view from the new tempo
        if (mTimelineState.GetCurrBeat() >= 0.0f)
        {
            UpdateUIFromBeat(mTimelineState.GetCurrBeat());
        }

        mEnableUndo = true;
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
    repaint();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnSaveFocusedObject()
{
    SaveTimeline();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SaveTimeline()
{
    if (mTimelineHandle.IsValid())
    {
        AssetIOMessageController::Message msg;
        msg.SetMessageType(AssetIOMessageController::Message::SAVE_ASSET);
        msg.SetObject(mTimelineHandle);
        SendAssetIoMessage(msg);
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
    //HACK: for now force open the timeline when the UI is ready.
    AssetIOMessageController::Message msg(AssetIOMessageController::Message::OPEN_ASSET); 
    msg.SetString(QString("Timeline/mainTimeline.pas"));
    SendAssetIoMessage(msg);
    mIsCursorQueued = false;
    mApplication = applicationProxy;
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData)
{
    if (!mTimelineHandle.IsValid())
    {
        mTimelineHandle = object;
        mTimelineState.SetState(initData.toMap());

        mEnableUndo = false;

        ui.addButton->setEnabled(true);
        ui.removeButton->setEnabled(true);
        ui.deleteButton->setEnabled(true);

        ui.playButton->setEnabled(true);
        ui.playButton->setChecked(false);
        ui.graphicsView->OnPlayModeToggled(false);

        ui.snapCombo->setEnabled(true);
        ui.snapCombo->setCurrentIndex(0);
        mSnapNumTicks = 1;
        UpdateUIFromBeat(0.0f);
        ui.graphicsView->setEnabled(true);

        ui.propertyGridWidget->SetApplicationProxy(GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy());
        ui.propertyGridWidget->SetCurrentProxy(mTimelineHandle);

        ui.bpmSpin->setValue(static_cast<double>(mTimelineState.GetBeatsPerMinute()));
        ui.bpmSpin->setEnabled(true);

        if (mTimelineState.HasMasterScript())
        {
            OnShowActiveTimelineButton(true, mTimelineState.GetMasterScriptPath());
        }

        ui.graphicsView->SetTimeline(&mTimelineState);
        // Update the content of the timeline graphics view from the timeline of the app
        ui.graphicsView->RefreshFromTimeline();

        mEnableUndo = true;
    }
    else
    {
        ED_LOG("Multiple timeline editing of pegasus is not supported on this version.");
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnShowActiveTimelineButton(bool shouldShowActiveScript, QString script)
{
    ui.removeScriptButton->setEnabled(shouldShowActiveScript);
    mLoadedScript = script; //cache the name so we can reopen it easily
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnUIForAppClosed()
{
    mApplication = nullptr;
    mUndoStack->clear();
    ui.addButton->setEnabled(false);
    ui.removeButton->setEnabled(false);
    ui.deleteButton->setEnabled(false);
    ui.removeScriptButton->setEnabled(false);

    ui.playButton->setEnabled(false);
    ui.playButton->setChecked(false);

    ui.propertyGridWidget->ClearProperties();

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
    ui.graphicsView->SetTimeline(nullptr);
    mTimelineHandle = AssetInstanceHandle();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBeatsPerMinuteChanged(double bpm)
{
    if (mEnableUndo)
    {
        if (mTimelineHandle.IsValid())
        {
            // Create the undo command
            TimelineSetBPMUndoCommand * undoCommand = new TimelineSetBPMUndoCommand(
               static_cast<double>(mTimelineState.GetBeatsPerMinute()), bpm, this);
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
    
    if (mTimelineHandle.IsValid())
    {
        unsigned int numTicksPerBeat = mTimelineState.GetNumTicksPerBeat();

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

//----------------------------------------------------------------------------------------
    
void TimelineDockWidget::SetCurrentBeat(float beat)
{
    if (mTimelineHandle.IsValid() && !mIsCursorQueued)
    {
        //prevent more UI messages to be sent until we have completed the request.
        mIsCursorQueued = true;
        TimelineIOMessageController::Message msg(TimelineIOMessageController::Message::SET_PARAMETER);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetTarget(TimelineIOMessageController::TIMELINE_OBJECT);
        msg.SetParameterName(static_cast<unsigned>(ShadowTimelineState::PROP_CURR_BEAT));
        msg.SetObserver(&mObserver);
        QVariant arg(beat);
        msg.SetArg(arg);
        SendTimelineIoMessage(msg);
    }
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

void TimelineDockWidget::OnBlockSelected(unsigned blockGuid)
{
    ui.propertyGridWidget->SetCurrentTimelineBlock(mTimelineHandle, blockGuid, QString(""));
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnMultiBlocksSelected()
{
    ui.propertyGridWidget->ClearProperties();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlocksDeselected()
{
    ui.propertyGridWidget->SetCurrentProxy(mTimelineHandle);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlockDoubleClicked(QString blockScriptToOpen)
{
    if (mApplication != nullptr && blockScriptToOpen != "")
    {
        AssetIOMessageController::Message msg(AssetIOMessageController::Message::OPEN_ASSET);
        msg.SetString(blockScriptToOpen);
        SendAssetIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::Observer::OnParameterUpdated(const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned targetObj, unsigned parameterName, const QVariant& parameterValue)
{
    if (timelineHandle == mDockWidget->mTimelineHandle)
    {
        if (static_cast<unsigned>(TimelineIOMessageController::TIMELINE_OBJECT) == targetObj)
        {
            if (static_cast<unsigned>(ShadowTimelineState::PROP_CURR_BEAT) == parameterName)
            {
                bool success = false;
                float beat = parameterValue.toFloat(&success);
                ED_ASSERT(success);
                mDockWidget->UpdateUIFromBeat(beat);
                mDockWidget->mIsCursorQueued = false; //we are done queing cursor. Lets chill now.
            }

            //update the shadow state
            mDockWidget->mTimelineState.GetRootState()[ShadowTimelineState::Str(static_cast<ShadowTimelineState::PropName>(parameterName))] = parameterValue;
        }
    }
}


void TimelineDockWidget::Observer::OnBlockOpResponse(const TimelineIOMessageController::BlockOpResponse& r)
{
    if (r.op == TimelineIOMessageController::ASK_POSITION)
    {
        if (r.success)
        {
            TimelineBlockGraphicsItem* blockItem = nullptr;
            unsigned oldLane = mDockWidget->ui.graphicsView->FindLane(r.blockGuid, blockItem);
            ED_ASSERT(blockItem != nullptr);            
            if (blockItem)
            {
                 
                TimelineSetBlockPositionUndoCommand* undoCmd = new TimelineSetBlockPositionUndoCommand(blockItem, r.newLane == -1 ? oldLane : (unsigned)r.newLane, r.newBeat, r.mouseClickId, mDockWidget);
                mDockWidget->mUndoStack->push(undoCmd);
            }
        }
    }
    else if (r.op == TimelineIOMessageController::MOVE)
    {
        if (r.success)
        {
            mDockWidget->ui.graphicsView->UpdateBeatVisuals(r.newLane,r.blockGuid,r.newBeat);
        }
    }
}


