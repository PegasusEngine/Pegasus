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
#include "MessageControllers/TimelineIOMessageController.h"

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"


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
    TimelineIOMessageController::Message undoMsg(TimelineIOMessageController::Message::BLOCK_OPERATION);
    undoMsg.SetBlockOp(TimelineIOMessageController::MOVE);
    undoMsg.SetTimelineHandle(mMessenger->GetTimelineAssetHandle());
    undoMsg.SetBlockGuid(blockState.GetGuid());
    undoMsg.SetLaneId(blockItem->GetLane());
    undoMsg.SetObserver(mMessenger->GetObserver());
    undoMsg.SetArg(QVariant(blockItem->GetBeat()));

    //ask the render thread runtime the best available position for the candidate beat & lane.
    TimelineIOMessageController::Message redoMsg(TimelineIOMessageController::Message::BLOCK_OPERATION);
    redoMsg.SetBlockOp(TimelineIOMessageController::MOVE);
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

const TimelineIOMessageController::Message* TimelineSetBlockPositionUndoCommand::FindMessage(bool isUndoStack, unsigned blockGuid) const
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
        const TimelineIOMessageController::Message* thisMsg = FindMessage(true, it.value().GetBlockGuid());
        if (thisMsg == nullptr)
        {
            mUndoMessages.insert(it.value().GetBlockGuid(), it.value());
        }
    }

    for (BlockMessageMap::const_iterator it = otherCmd->mRedoMessages.begin(); it != otherCmd->mRedoMessages.end(); ++it)
    {
        const TimelineIOMessageController::Message* thisMsg = FindMessage(false, it.value().GetBlockGuid());
        mRedoMessages.insert(it.value().GetBlockGuid(), it.value());
    }


    return true;
}
