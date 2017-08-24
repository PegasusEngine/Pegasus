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
 
#include "ui_TimelineDockWidget.h"

#include <MessageControllers\MsgDefines.h>
#include <QGraphicsObject>
#include <QMessagebox>
#include <QFileDialog>

#include <QSignalMapper>
#include <QListWidgetItem>
#include <QUndoStack>
#include <QMenu>

class TdwObserver : public TimelineIOMessageObserver
{
public:
    explicit TdwObserver(TimelineDockWidget* dockWidget)
    : mDockWidget(dockWidget)
    {
    }

    virtual ~TdwObserver(){}

    virtual void OnParameterUpdated(const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, const QVariant& parameterValue);

    virtual void OnBlockOpResponse(const TimelineIOMCBlockOpResponse& response);

    virtual bool OnRedrawTimelineBeat(AssetInstanceHandle timelineHandle, float beat);

private:
    TimelineDockWidget* mDockWidget;
} ;



TimelineDockWidget::TimelineDockWidget(QWidget *parent, Editor* editor)
    :   
    PegasusDockWidget(parent, editor),
    mSnapNumTicks(1),
    mEnableUndo(true),
    mApplication(nullptr),
    mIsCursorQueued(false),
    mUi(nullptr),
    mAvailableBlocksNamesCount(0),
    mCurrentBlockSelected(~0)
{
    mObserver = new TdwObserver(this);
    mUi = new Ui::TimelineDockWidget();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetupUi()
{
    // Set the initial UI state
    mUi->setupUi(this);
    mUi->propertyGridWidget->SetMessenger(this);

    mUndoStack = new QUndoStack(this);
    
    mAppAvailableBlocksMenu = new QMenu();
    mAppAvailableBlocksSignalMapper = new QSignalMapper();
    mUi->addNewBlockButton->setMenu(mAppAvailableBlocksMenu);
    mUi->addNewBlockButton->setPopupMode(QToolButton::InstantPopup);
    
    UpdateUIForAppClosed();

	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    mUi->graphicsView->SetUndoStack(GetCurrentUndoStack());
    mUi->graphicsView->installEventFilter(this);

    // Make connections between the UI elements and the child views
    connect(mUi->saveButton, SIGNAL(clicked()),
            this, SLOT(SaveTimeline()));
    connect(mUi->addLaneButton, SIGNAL(clicked()),
            mUi->graphicsView, SLOT(AddLane()));
    connect(mUi->playButton, SIGNAL(toggled(bool)),
            mUi->graphicsView, SLOT(OnPlayModeToggled(bool)));
    connect(mUi->deleteBlocksButton, SIGNAL(clicked()),
            this, SLOT(RequestDeleteSelectedBlocks()));
    connect(mUi->bpmSpin, SIGNAL(valueChanged(double)),
            this, SLOT(OnBeatsPerMinuteChanged(double)));
    connect(mUi->snapCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnSnapModeChanged(int)));
    connect(mUi->graphicsView, SIGNAL(BeatUpdated(float)),
            this, SLOT(SetCurrentBeat(float)));
    connect(mUi->graphicsView, SIGNAL(BlockSelected(unsigned)),
            this, SLOT(OnBlockSelected(unsigned)));
    connect(mUi->graphicsView, SIGNAL(MultiBlocksSelected()),
            this, SLOT(OnMultiBlocksSelected()));
    connect(mUi->graphicsView, SIGNAL(BlocksDeselected()),
            this, SLOT(OnBlocksDeselected()));
    connect(mUi->graphicsView, SIGNAL(RequestDrawAllViewports()),
            this, SIGNAL(OnRequestRedrawAllViewports()));
    connect(mUi->graphicsView, SIGNAL(RequestChangeScript(QGraphicsObject*,unsigned)),
            this, SLOT(RequestChangeScript(QGraphicsObject*,unsigned)));
    connect(mUi->graphicsView, SIGNAL(RequestRemoveScript(QGraphicsObject*,unsigned)),
            this, SLOT(RequestRemoveScript(QGraphicsObject*,unsigned)));
    connect(mUi->graphicsView, SIGNAL(RequestBlockMove(QGraphicsObject*, QPointF)),
            this, SLOT(RequestMoveBlock(QGraphicsObject*, QPointF)));
    connect(mUi->graphicsView, SIGNAL(BlockDoubleClicked(QString)),
            this, SLOT(OnBlockDoubleClicked(QString)));

    connect(mUi->playButton, SIGNAL(toggled(bool)),
            this, SLOT(OnPlayModeToggled(bool)));

    connect(mUi->propertyGridWidget, SIGNAL(OnPropertyUpdated(QtProperty*)),
            this, SLOT(OnPropertyUpdated(QtProperty*)));

    connect(
       mAppAvailableBlocksSignalMapper, SIGNAL(mapped(int)),
       this, SLOT(OnCreateNewBlock(int)));

    mMasterScriptMenu = new QMenu();
    mOpenMasterScriptButton   = mMasterScriptMenu->addAction(tr("Open/Replace new master script."));
    mRemoveMasterScriptButton = mMasterScriptMenu->addAction(tr("Remove master script."));
    mEditMasterScriptButton   = mMasterScriptMenu->addAction(tr("Edit master script."));
    mUi->scriptOptionsButton->setMenu(mMasterScriptMenu);
    mUi->scriptOptionsButton->setPopupMode(QToolButton::InstantPopup);

    connect(mOpenMasterScriptButton, SIGNAL(triggered()),
            this, SLOT(RequestMasterTimelineScriptLoad()));

    connect(mEditMasterScriptButton, SIGNAL(triggered()),
            this, SLOT(EditMasterScript()));

    connect(mRemoveMasterScriptButton, SIGNAL(triggered()),
            this, SLOT(RemoveMasterScript()));

    //! Menu button for script open / removal
    mMusicOptionsMenu = new QMenu();
    mOpenNewMusicFile = mMusicOptionsMenu->addAction(tr("Open/Replace new music file."));
    mRemoveMusicFile  = mMusicOptionsMenu->addAction(tr("Remove music file."));
    mUi->musicOptionsButton->setMenu(mMusicOptionsMenu);
    mUi->musicOptionsButton->setPopupMode(QToolButton::InstantPopup);

    connect(mOpenNewMusicFile, SIGNAL(triggered()),
            this, SLOT(OpenMusicFile()));

    connect(mRemoveMusicFile, SIGNAL(triggered()),
            this, SLOT(RemoveMusicFile()));

    connect(mUi->muteUnmuteButton, SIGNAL(clicked(bool)),
            this, SLOT(MuteUnmuteToggle(bool)));
}

//----------------------------------------------------------------------------------------

TimelineDockWidget::~TimelineDockWidget()
{
    delete mUi;
    delete mObserver;
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnPropertyUpdated(QtProperty* property)
{
    bool isName = property->propertyName() == "Name";
    bool isColor = !isName  && property->propertyName() == "Color";
    bool isBeat =  !isColor && property->propertyName() == "Beat";
    bool isDuration = !isBeat && property->propertyName() == "Duration";

    bool isVisualProp = isName || isColor || isBeat || isDuration;

    if (isVisualProp)
    {
        TimelineBlockGraphicsItem* item = nullptr;
        mUi->graphicsView->FindLane(mCurrentBlockSelected, item);
        if (item != nullptr)
        {
            ShadowBlockState& blockState = item->EditBlockProxy();
            ShadowBlockState::PropName targetName;
            QVariant targetValue;
            if (isName)
            {
                targetName = ShadowBlockState::PROP_BLOCK_NAME;
                targetValue = mUi->propertyGridWidget->GetS64Property(property);
            }
            else if (isColor)
            {
                targetName = ShadowBlockState::PROP_BLOCK_COLOR;
                QColor c = mUi->propertyGridWidget->GetRgbProperty(property);
                int intColVal = (c.red() << 24) | (c.green() << 16) | (c.blue() << 8);
                targetValue = intColVal;
            }
            else if (isBeat)
            {
                targetName = ShadowBlockState::PROP_BLOCK_BEAT;
                targetValue = mUi->propertyGridWidget->GetUintProperty(property);
            }
            else if (isDuration)
            {
                targetName = ShadowBlockState::PROP_BLOCK_DURATION;
                targetValue = mUi->propertyGridWidget->GetUintProperty(property);
            }
            blockState.GetRootState().insert(ShadowBlockState::Str(targetName), targetValue);

            //redraw and flush visual properties
            QRectF rect = item->boundingRect();
            rect.translate(item->pos());
            mUi->graphicsView->invalidateScene(rect);
            item->FlushVisualProperties();
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnRepaintTimeline()
{
    mUi->graphicsView->RedrawInternalBlocks();
}

//----------------------------------------------------------------------------------------

QString TimelineDockWidget::AskForFile(const QString& title, const QString& extensionFormat)
{
    QString rootFolder = mApplication->GetAssetsRoot();
    QDir qd(rootFolder);
    QString requestedFile = QFileDialog::getOpenFileName(this, title, rootFolder, extensionFormat);
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

QString TimelineDockWidget::AskForTimelineScript()
{
    QString windowTitle = tr("Open BlockScript.");
    QString fileExtension = tr("%1 BlockScript (*.%2)").arg(tr(Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeName), tr(Pegasus::ASSET_TYPE_BLOCKSCRIPT.mExtension));
    return AskForFile(windowTitle, fileExtension);
}

//----------------------------------------------------------------------------------------

QString TimelineDockWidget::AskForMusicFile()
{
    QString windowTitle = tr("Open Music File.");
    QString fileExtension = tr("Music File (*.mp3;*.wav;*.ogg)");
    return AskForFile(windowTitle, fileExtension);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestChangeScript(QGraphicsObject* sender, unsigned blockGuid)
{
    if (mApplication != nullptr)
    {
        QString requestedScript = AskForTimelineScript();
        if (requestedScript != "")
        {
            TimelineIOMCMessage msg(TimelineIOMCMessage::SET_BLOCKSCRIPT);
            msg.SetBlockGuid(blockGuid);
            msg.SetString(requestedScript);
            SendTimelineIoMessage(msg);
            static_cast<TimelineBlockGraphicsItem*>(sender)->UIUpdateBlockScript(requestedScript);
        }
    }
}

//----------------------------------------------------------------------------------------

bool TimelineDockWidget::IsPlaying() const
{
    return mUi->playButton->isChecked();
}

//----------------------------------------------------------------------------------------

bool TimelineDockWidget::HasFocus() const
{
    return hasFocus() || mUi->graphicsView->hasFocus(); 
}
//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestRemoveScript(QGraphicsObject* sender, unsigned blockGuid)
{
    if (mApplication != nullptr)
    {
        TimelineIOMCMessage msg(TimelineIOMCMessage::CLEAR_BLOCKSCRIPT);
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
        TimelineIOMCMessage msg(TimelineIOMCMessage::BLOCK_OPERATION);
        TimelineBlockGraphicsItem* graphicsItem = static_cast<TimelineBlockGraphicsItem*>(sender);
        const ShadowBlockState& blockState = graphicsItem->GetBlockProxy();
        msg.SetBlockOp(ASK_BLOCK_POSITION);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetBlockGuid(blockState.GetGuid());
        msg.SetLaneId(graphicsItem->GetLaneFromY(amount.y()));
        msg.SetObserver(mObserver);
        msg.SetArg(QVariant(graphicsItem->GetBeatFromX(amount.x())));
        msg.SetMouseClickId(TimelineBlockGraphicsItem::sMouseClickID);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnDeleteFocusedObject()
{
    RequestDeleteSelectedBlocks();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestDeleteSelectedBlocks()
{
    QList<QGraphicsItem*> selectedItems = mUi->graphicsView->scene()->selectedItems();
    QVariantList blockGuids;
    foreach (QGraphicsItem* i, selectedItems)
    {
        TimelineBlockGraphicsItem* blockGraphicsItem = static_cast<TimelineBlockGraphicsItem*>(i);
        blockGuids.push_back(blockGraphicsItem->GetBlockProxy().GetGuid());
    }

    TimelineIOMCMessage msg(TimelineIOMCMessage::BLOCK_OPERATION);
    msg.SetBlockOp(ASK_DELETE_BLOCKS);
    msg.SetTimelineHandle(mTimelineHandle);
    msg.SetArg(blockGuids);
    msg.SetObserver(GetObserver());
    SendTimelineIoMessage(msg);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnPlayModeToggled(bool enabled)
{
    mUi->graphicsView->OnPlayModeToggled(enabled);
    TimelineIOMCMessage msg(TimelineIOMCMessage::TOGGLE_PLAY_MODE);
    msg.SetObserver(GetObserver());
    msg.SetTimelineHandle(mTimelineHandle);
    msg.SetIsPlayMode(enabled);
    SendTimelineIoMessage(msg);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnFocusBlock(unsigned blockGuid)
{
    setFocus();
    mUi->graphicsView->OnFocusBlock(blockGuid);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RequestMasterTimelineScriptLoad()
{
    if (mApplication != nullptr)
    {
        QString requestedScript = AskForTimelineScript();
        if (requestedScript != "")
        {
            TimelineIOMCMessage msg(TimelineIOMCMessage::SET_MASTER_BLOCKSCRIPT);
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
        TimelineIOMCMessage msg(TimelineIOMCMessage::CLEAR_MASTER_BLOCKSCRIPT);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OpenMusicFile()
{
    if (mApplication != nullptr)
    {
        QString requestedFile = AskForMusicFile();
        if (requestedFile != "")
        {
            TimelineIOMCMessage msg(TimelineIOMCMessage::SET_MUSIC_FILE);
            msg.SetString(requestedFile);
            SendTimelineIoMessage(msg);
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::RemoveMusicFile()
{
    if (mApplication != nullptr)
    {
        TimelineIOMCMessage msg(TimelineIOMCMessage::CLEAR_MUSIC_FILE);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::MuteUnmuteToggle(bool isNotMuted)
{
    if (mApplication != nullptr)
    {
        TimelineIOMCMessage msg(TimelineIOMCMessage::SET_DEBUG_ENABLE_MUSIC);
        msg.SetIsPlayMode(isNotMuted);//just recyling this bool
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::EditMasterScript()
{
    if (mApplication != nullptr && mLoadedScript != "")
    {
        AssetIOMCMessage msg(AssetIOMCMessage::OPEN_ASSET);
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
    if (mUi->graphicsView == obj)
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
    mUi->graphicsView->EnableAntialiasing(enable);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetBeatsPerMinute(double bpm)
{
    if (mTimelineHandle.IsValid())
    {
        mEnableUndo = false;

        // Update the tempo field
        mUi->bpmSpin->setValue(bpm);

        // Apply the new tempo to the timeline
        QVariant bpmArg(bpm);
        TimelineIOMCMessage msg(TimelineIOMCMessage::SET_PARAMETER);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetParameterName((unsigned)ShadowTimelineState::PROP_BEATS_PER_MIN);
        msg.SetArg(bpmArg);
        msg.SetTarget(TIMELINE_OBJECT);
        msg.SetObserver(mObserver);
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
    const double timeMinutes = static_cast<double>(beat) / mUi->bpmSpin->value();
    const unsigned int intMinutes = static_cast<unsigned int>(floor(timeMinutes));
    const double fracTimeSeconds = (timeMinutes - static_cast<double>(intMinutes)) * 60.0;
    const unsigned int intSeconds = static_cast<unsigned int>(floor(fracTimeSeconds));
    const double fracTimeMilliseconds = (fracTimeSeconds - static_cast<double>(intSeconds)) * 1000.0;
    const unsigned int intMilliseconds = static_cast<unsigned int>(floor(fracTimeMilliseconds));
    SetTimeLabel(intMinutes, intSeconds, intMilliseconds);

    // Place the cursor in the right position
    mUi->graphicsView->SetCursorFromBeat(beat);    
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
        AssetIOMCMessage msg;
        msg.SetMessageType(AssetIOMCMessage::SAVE_ASSET);
        msg.SetObject(mTimelineHandle);
        SendAssetIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnReceiveAssetIoMessage(AssetIOMCMessage::IoResponseMessage msg)
{
    switch(msg)
    {
    case AssetIOMCMessage::IO_SAVE_ERROR:
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
    AssetIOMCMessage msg(AssetIOMCMessage::OPEN_ASSET); 
    msg.SetString(QString("Timeline/mainTimeline.pas"));
    SendAssetIoMessage(msg);
    mIsCursorQueued = false;
    mApplication = applicationProxy;

    QIcon blockIcon(tr(":TypeIcons/timelinescript.png"));

    Pegasus::Timeline::ITimelineManagerProxy* t = applicationProxy->GetTimelineManagerProxy();
    mAvailableBlocksNamesCount = t->GetRegisteredBlockNames(mAvailableBlocksClassNames, mAvailableBlocksEditorNames);

    for (unsigned indx = 0; indx < mAvailableBlocksNamesCount; ++indx)
    {
        QAction* a = mAppAvailableBlocksMenu->addAction(blockIcon,mAvailableBlocksEditorNames[indx]);
        mAppAvailableBlocksSignalMapper->setMapping(a, indx);
        mAvailableBlocksActions.push_back(a);
        connect(a, SIGNAL(triggered()),
            mAppAvailableBlocksSignalMapper, SLOT(map()));
    }

}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnOpenObject(AssetInstanceHandle object, const QString& displayName, const QVariant& initData)
{
    if (!mTimelineHandle.IsValid())
    {
        mTimelineHandle = object;
        mTimelineState.SetState(initData.toMap());

        mEnableUndo = false;

        mUi->addLaneButton->setEnabled(true);
        mUi->deleteLaneButton->setEnabled(true);
        mUi->addNewBlockButton->setEnabled(true);
        mUi->deleteBlocksButton->setEnabled(true);

        mUi->playButton->setEnabled(true);
        mUi->playButton->setChecked(false);
        mUi->graphicsView->OnPlayModeToggled(false);

        mUi->musicOptionsButton->setEnabled(true);

        mUi->snapCombo->setEnabled(true);
        mUi->snapCombo->setCurrentIndex(0);
        mSnapNumTicks = 1;
        UpdateUIFromBeat(0.0f);
        mUi->graphicsView->setEnabled(true);
        mUi->muteUnmuteButton->setEnabled(true);

        mUi->propertyGridWidget->SetApplicationProxy(GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy());
        mUi->propertyGridWidget->SetCurrentProxy(mTimelineHandle);

        mUi->bpmSpin->setValue(static_cast<double>(mTimelineState.GetBeatsPerMinute()));
        mUi->bpmSpin->setEnabled(true);

        mUi->scriptOptionsButton->setEnabled(true);

        if (mTimelineState.HasMasterScript())
        {
            OnShowActiveTimelineButton(true, mTimelineState.GetMasterScriptPath());
        }

        mUi->graphicsView->SetTimeline(&mTimelineState);
        //remove the fake lane 0
        // Update the content of the timeline graphics view from the timeline of the app
        mUi->graphicsView->RefreshFromTimeline();

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
    mEditMasterScriptButton->setEnabled(shouldShowActiveScript);
    mRemoveMasterScriptButton->setEnabled(shouldShowActiveScript);
    mLoadedScript = script; //cache the name so we can reopen it easily
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnUIForAppClosed()
{
    foreach(QAction* a, mAvailableBlocksActions)
    {
        mAppAvailableBlocksSignalMapper->removeMappings(a);
    }
    mAppAvailableBlocksMenu->clear();
    mAvailableBlocksActions.clear();
    mAvailableBlocksNamesCount  = 0;

    mApplication = nullptr;
    mUndoStack->clear();
    mUi->addLaneButton->setEnabled(false);
    mUi->deleteLaneButton->setEnabled(false);
    mUi->scriptOptionsButton->setEnabled(false);
    mUi->addNewBlockButton->setEnabled(false);
    mUi->deleteBlocksButton->setEnabled(false);
    mUi->playButton->setEnabled(false);
    mUi->playButton->setChecked(false);
    mUi->musicOptionsButton->setEnabled(false);
    mUi->muteUnmuteButton->setEnabled(false);

    mUi->propertyGridWidget->ClearProperties();

    // Clear the content of the timeline graphics view and set the default timeline
    mUi->graphicsView->Initialize();
    mUi->graphicsView->OnPlayModeToggled(false);

    mUi->bpmSpin->setEnabled(false);
    mUi->bpmSpin->setValue(138.0);
    mUi->snapCombo->setEnabled(false);
    mUi->snapCombo->setCurrentIndex(0);
    mSnapNumTicks = 1;
    UpdateUIFromBeat(0.0f);
    mUi->graphicsView->setEnabled(false);
    mUi->graphicsView->SetTimeline(nullptr);

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
        TimelineIOMCMessage msg(TimelineIOMCMessage::SET_PARAMETER);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetTarget(TIMELINE_OBJECT);
        msg.SetParameterName(static_cast<unsigned>(ShadowTimelineState::PROP_CURR_BEAT));
        msg.SetObserver(mObserver);
        QVariant arg(beat);
        msg.SetArg(arg);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetBeatLabel(unsigned int beat, unsigned int subBeat, unsigned int subSubBeat)
{
    mUi->beatLabel->setText(QString("%1:%2.%3").arg(beat).arg(subBeat).arg(subSubBeat));
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::SetTimeLabel(unsigned int minutes, unsigned int seconds, unsigned int milliseconds)
{
    mUi->timeLabel->setText(QString("%1:%2:%3").arg(minutes).arg(seconds, 2, 10, QChar('0')).arg(milliseconds, 3, 10, QChar('0')));
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlockSelected(unsigned blockGuid)
{
    mCurrentBlockSelected = blockGuid;
    mUi->propertyGridWidget->SetCurrentTimelineBlock(mTimelineHandle, blockGuid, QString(""));
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnMultiBlocksSelected()
{
    mCurrentBlockSelected = ~0;
    mUi->propertyGridWidget->ClearProperties();
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlocksDeselected()
{
    mCurrentBlockSelected = ~0;
    mUi->propertyGridWidget->SetCurrentProxy(mTimelineHandle);
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnBlockDoubleClicked(QString blockScriptToOpen)
{
    if (mApplication != nullptr && blockScriptToOpen != "")
    {
        AssetIOMCMessage msg(AssetIOMCMessage::OPEN_ASSET);
        msg.SetString(blockScriptToOpen);
        SendAssetIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TimelineDockWidget::OnCreateNewBlock(int blockTypeId)
{
    if (blockTypeId >= 0 && blockTypeId < static_cast<int>(mAvailableBlocksNamesCount))
    {
        unsigned initialBeat = static_cast<unsigned> (mTimelineState.GetCurrBeat() * static_cast<float>(mTimelineState.GetNumTicksPerBeat()));
        QString blockClassTypeName = tr(mAvailableBlocksClassNames[blockTypeId]);
        TimelineIOMCMessage msg(TimelineIOMCMessage::BLOCK_OPERATION);
        msg.SetBlockOp(ASK_NEW_BLOCK);

        //these are the message contents for the constructor of a block.
        //These have to match to those expected in TimelineIOMessageController.cpp:CreateAndInsertNewBlock
        QVariantMap newblockArgs;

        newblockArgs.insert(tr("className"),blockClassTypeName);
        newblockArgs.insert(tr("initialBeat"),(unsigned)initialBeat);
        newblockArgs.insert(tr("initialDuration"),(unsigned)1024);
        newblockArgs.insert(tr("targetLane"),(unsigned)0);
        msg.SetTimelineHandle(mTimelineHandle);
        msg.SetObserver(mObserver);
        msg.SetArg(newblockArgs);
        SendTimelineIoMessage(msg);
    }
}

//----------------------------------------------------------------------------------------

void TdwObserver::OnParameterUpdated(const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned targetObj, unsigned parameterName, const QVariant& parameterValue)
{
    if (timelineHandle == mDockWidget->mTimelineHandle)
    {
        if (static_cast<unsigned>(TIMELINE_OBJECT) == targetObj)
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


void TdwObserver::OnBlockOpResponse(const TimelineIOMCBlockOpResponse& r)
{
    if (r.success)
    {
        if (r.op == ASK_BLOCK_POSITION)
        {
            TimelineBlockGraphicsItem* blockItem = nullptr;
            unsigned oldLane = mDockWidget->mUi->graphicsView->FindLane(r.blockGuid, blockItem);
            ED_ASSERT(blockItem != nullptr);            
            if (blockItem)
            {
                 
                TimelineSetBlockPositionUndoCommand* undoCmd = new TimelineSetBlockPositionUndoCommand(blockItem, r.newLane == -1 ? oldLane : (unsigned)r.newLane, r.newBeat, r.mouseClickId, mDockWidget);
                mDockWidget->mUndoStack->push(undoCmd);
            }
        }
        else if (r.op == MOVE_BLOCK)
        {
            mDockWidget->mUi->graphicsView->UpdateBeatVisuals(r.newLane,r.blockGuid,r.newBeat);
        }
        else if (r.op == NEW_BLOCK) 
        {
            mDockWidget->mUi->graphicsView->RefreshLaneFromTimelineLane(r.newLane, r.newShadowLaneState);
            if (r.requiresRefocus)
            {
                mDockWidget->OnFocusBlock(r.blockGuid);
            }
        }
        else if (r.op == ASK_NEW_BLOCK) 
        {
            //now that we have the predicted block guid, we resend this message to ask for a new block.
            TimelineCreateBlockUndoCommand* createUndoCmd = new TimelineCreateBlockUndoCommand(r.timelineHandle, r.blockGuid, mDockWidget, r.arg);
            mDockWidget->mUndoStack->push(createUndoCmd);
        }
        else if (r.op == ASK_DELETE_BLOCKS)
        {
            QVariantMap args = r.arg.toMap();
            QVariantList blockGuids = args[QString("guids")].toList();
            QVariantList blockJsonStates = args[QString("jsonStates")].toList();
            QVector<const ShadowBlockState*> shadowStates;
            QVector<int> targetLanes;
            for (int i = 0; i < blockGuids.size(); ++i)
            {
                TimelineBlockGraphicsItem* graphicsItem = nullptr;
                int targetLane = mDockWidget->mUi->graphicsView->FindLane(blockGuids[i].toUInt(), graphicsItem);
                shadowStates.push_back(&graphicsItem->GetBlockProxy());
                targetLanes.push_back(targetLane);
            }
            TimelineDeleteBlockUndoCommand* deleteUndoCommand = new TimelineDeleteBlockUndoCommand(r.timelineHandle, shadowStates, targetLanes, blockJsonStates, mDockWidget);
            mDockWidget->mUndoStack->push(deleteUndoCommand);
        }
        else if (r.op == DELETE_BLOCKS)
        {
            foreach (int lane, r.lanesFound)
            {
                ShadowLaneState laneState = r.lanesFoundState[lane];
                mDockWidget->mUi->graphicsView->RefreshLaneFromTimelineLane(lane, laneState);
            }
        }
    }
}

bool TdwObserver::OnRedrawTimelineBeat(AssetInstanceHandle timelineHandle, float beat)
{
    if (timelineHandle == mDockWidget->mTimelineHandle)
    {
        mDockWidget->UpdateUIFromBeat(beat);
        return mDockWidget->IsPlaying();
    }
    return false;
}



