/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineUndoCommands.cpp
//! \author	Karolyn Boulanger
//! \date	20th January 2014
//! \brief	List of timeline commands that can be undone

#include "Timeline/TimelineUndoCommands.h"
#include "Timeline/TimelineBlockGraphicsItem.h"
#include "Timeline/TimelineDockWidget.h"

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "MessageControllers/TimelineIOMessageController.h"


#include <QObject>


TimelineSetBPMUndoCommand::TimelineSetBPMUndoCommand(double previousBPM, double newBPM, TimelineDockWidget* messenger, QUndoCommand * parent)
:   QUndoCommand(parent),
    mPreviousBPM(previousBPM),
    mNewBPM(newBPM),
    mMessenger(messenger)
{
    ED_ASSERTSTR(previousBPM > 0.0, "Invalid previous tempo, it should be positive");
    ED_ASSERTSTR(newBPM > 0.0, "Invalid new tempo, it should be positive");
    ED_ASSERTSTR(newBPM != previousBPM, "Invalid new tempo, it should different from the previous tempo");

    setText(QObject::tr("Set Tempo to %1").arg(newBPM, 0, 'f', 2));
}

//----------------------------------------------------------------------------------------

void TimelineSetBPMUndoCommand::undo()
{
    mMessenger->SetBeatsPerMinute(mPreviousBPM);
}

//----------------------------------------------------------------------------------------

void TimelineSetBPMUndoCommand::redo()
{
    mMessenger->SetBeatsPerMinute(mNewBPM);
}

//----------------------------------------------------------------------------------------

TimelineSetBlockPositionUndoCommand::TimelineSetBlockPositionUndoCommand(TimelineBlockGraphicsItem * blockItem,
                                                                         unsigned int newLane,
                                                                         Pegasus::Timeline::Beat newBeat,
                                                                         unsigned int mouseClickID,
                                                                         TimelineDockWidget* messenger,
                                                                         QUndoCommand * parent)
:   QUndoCommand(parent),
    mMouseClickID(mouseClickID),
    mMessenger(messenger)
{

    const ShadowBlockState& blockState = blockItem->GetBlockProxy();

    //ask the render thread runtime the best available position for the candidate beat & lane.
    TimelineIOMCMessage undoMsg(TimelineIOMCMessage::BLOCK_OPERATION);
    undoMsg.SetBlockOp(MOVE_BLOCK);
    undoMsg.SetTimelineHandle(mMessenger->GetTimelineAssetHandle());
    undoMsg.SetBlockGuid(blockState.GetGuid());
    undoMsg.SetLaneId(blockItem->GetLane());
    undoMsg.SetObserver(mMessenger->GetObserver());
    undoMsg.SetArg(QVariant(blockItem->GetBeat()));

    //ask the render thread runtime the best available position for the candidate beat & lane.
    TimelineIOMCMessage redoMsg(TimelineIOMCMessage::BLOCK_OPERATION);
    redoMsg.SetBlockOp(MOVE_BLOCK);
    redoMsg.SetTimelineHandle(mMessenger->GetTimelineAssetHandle());
    redoMsg.SetBlockGuid(blockState.GetGuid());
    redoMsg.SetLaneId(newLane);
    redoMsg.SetObserver(mMessenger->GetObserver());
    redoMsg.SetArg(QVariant(newBeat));

    mRedoMessages.insert(blockState.GetGuid(), redoMsg);
    mUndoMessages.insert(blockState.GetGuid(), undoMsg);
}

//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::ExecuteMessages(bool isUndoStack)
{
    BlockMessageMap& targetMap = isUndoStack ? mUndoMessages : mRedoMessages;
    for (BlockMessageMap::iterator it = targetMap.begin(); it != targetMap.end(); ++it)
    {
        mMessenger->SendTimelineIoMessage(it.value());
    }
}
//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::undo()
{
    ExecuteMessages(true);
}

//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::redo()
{
    ExecuteMessages(false);
}

//----------------------------------------------------------------------------------------

int TimelineSetBlockPositionUndoCommand::id() const
{
    // Create a unique ID by combining:
    // - a 24 bit ID for the moved block
    // - an 8 bit ID for the mouse click, changing each time the mouse click is released
    return mMouseClickID;
}

//----------------------------------------------------------------------------------------

const TimelineIOMCMessage* TimelineSetBlockPositionUndoCommand::FindMessage(bool isUndoStack, unsigned blockGuid) const
{
    const BlockMessageMap& targetMap = isUndoStack ? mUndoMessages : mRedoMessages;
    BlockMessageMap::const_iterator it = targetMap.find(blockGuid);
    if (it != targetMap.end())
    {
        return &it.value();
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------

bool TimelineSetBlockPositionUndoCommand::mergeWith(const QUndoCommand * command)
{
    //undo messages accumulate by getting only the oldest possible message. Discards new updates of a message.
    const TimelineSetBlockPositionUndoCommand* otherCmd = static_cast<const TimelineSetBlockPositionUndoCommand*>(command);

    for (BlockMessageMap::const_iterator it = otherCmd->mUndoMessages.begin(); it != otherCmd->mUndoMessages.end(); ++it)
    {
        const TimelineIOMCMessage* thisMsg = FindMessage(true, it.value().GetBlockGuid());
        if (thisMsg == nullptr)
        {
            mUndoMessages.insert(it.value().GetBlockGuid(), it.value());
        }
    }

    for (BlockMessageMap::const_iterator it = otherCmd->mRedoMessages.begin(); it != otherCmd->mRedoMessages.end(); ++it)
    {
        const TimelineIOMCMessage* thisMsg = FindMessage(false, it.value().GetBlockGuid());
        mRedoMessages.insert(it.value().GetBlockGuid(), it.value());
    }


    return true;
}

//----------------------------------------------------------------------------------------

TimelineCreateBlockUndoCommand::TimelineCreateBlockUndoCommand(
   const AssetInstanceHandle& timelineHandle,
   unsigned int targetBlockGuid,
   TimelineDockWidget* messenger,
   const QVariant& newBlockArgs,
   QUndoCommand * parent)
: QUndoCommand(parent), mMessenger(messenger)
{
    mCreateMessage.SetMessageType(TimelineIOMCMessage::BLOCK_OPERATION);
    mCreateMessage.SetBlockOp(NEW_BLOCK);
    mCreateMessage.SetBlockGuid(targetBlockGuid);
    mCreateMessage.SetTimelineHandle(timelineHandle);
    mCreateMessage.SetObserver(messenger->GetObserver());
    mCreateMessage.SetArg(newBlockArgs);

    QVariantList blockGuidsToDelete;
    blockGuidsToDelete.push_back(targetBlockGuid);
    mDeleteMessage.SetMessageType(TimelineIOMCMessage::BLOCK_OPERATION);
    mDeleteMessage.SetBlockOp(DELETE_BLOCKS);
    mDeleteMessage.SetArg(blockGuidsToDelete);
    mDeleteMessage.SetTimelineHandle(timelineHandle);
    mDeleteMessage.SetObserver(messenger->GetObserver());
}

void TimelineCreateBlockUndoCommand::redo()
{
    emit mMessenger->SendTimelineIoMessage(mCreateMessage);
}

void TimelineCreateBlockUndoCommand::undo()
{
    emit mMessenger->SendTimelineIoMessage(mDeleteMessage);
}

TimelineDeleteBlockUndoCommand::TimelineDeleteBlockUndoCommand(
   const AssetInstanceHandle& timelineHandle,
   const QVector<const ShadowBlockState*>& shadowBlockStates,
   const QVector<int>& targetLanes,
   const QVariantList& jsonStates,
   TimelineDockWidget* messenger,
   QUndoCommand * parent
)
: QUndoCommand(parent), mMessenger(messenger)
{
    //first figure out the deletion portion
    mDeleteMessage.SetMessageType(TimelineIOMCMessage::BLOCK_OPERATION);
    mDeleteMessage.SetBlockOp(DELETE_BLOCKS);
    mDeleteMessage.SetTimelineHandle(timelineHandle);
    mDeleteMessage.SetObserver(messenger->GetObserver());
    QVariantList blockGuidsToDelete;
    foreach (const ShadowBlockState* blockState, shadowBlockStates)
    {
        blockGuidsToDelete.push_back(blockState->GetGuid());
    }
    mDeleteMessage.SetArg(blockGuidsToDelete);

    TimelineIOMCMessage recreateMessage(TimelineIOMCMessage::BLOCK_OPERATION);
    recreateMessage.SetBlockOp(NEW_BLOCK);
    recreateMessage.SetTimelineHandle(timelineHandle);
    recreateMessage.SetObserver(messenger->GetObserver());
    for (int i = 0; i < shadowBlockStates.size(); ++i)
    {
        const ShadowBlockState* blockState = shadowBlockStates[i];
        int targetLane = targetLanes[i];

        QVariantMap newblockArgs;
        newblockArgs.insert(QString("className"),  blockState->GetClassName());
        newblockArgs.insert(QString("initialBeat"),blockState->GetBeat());
        newblockArgs.insert(QString("initialDuration"),blockState->GetDuration());
        newblockArgs.insert(QString("assignedGuid"),blockState->GetGuid());
        newblockArgs.insert(QString("targetLane"), targetLane);
        //TODO: drop here initial state of property grid as a json string.
        newblockArgs.insert(QString("jsonState"), jsonStates[i]);
        recreateMessage.SetArg(newblockArgs);
        recreateMessage.SetBlockGuid(blockState->GetGuid());
        mRecreateMessages.push_back(recreateMessage);
    }
}

void TimelineDeleteBlockUndoCommand::redo()
{
    emit mMessenger->SendTimelineIoMessage(mDeleteMessage);
}

void TimelineDeleteBlockUndoCommand::undo()
{
    foreach (const TimelineIOMCMessage& recreateMessage, mRecreateMessages)
    {
        emit mMessenger->SendTimelineIoMessage(recreateMessage);
    }
}

