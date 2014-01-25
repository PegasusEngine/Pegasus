/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineUndoCommands.cpp
//! \author	Kevin Boulanger
//! \date	20th January 2014
//! \brief	List of timeline commands that can be undone

#include "Timeline/TimelineUndoCommands.h"
#include "Timeline/TimelineBlockGraphicsItem.h"

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"

#include <QObject>


TimelineSetBPMUndoCommand::TimelineSetBPMUndoCommand(double previousBPM, double newBPM,
                                                     QUndoCommand * parent)
:   QUndoCommand(parent),
    mPreviousBPM(previousBPM),
    mNewBPM(newBPM)
{
    ED_ASSERTSTR(previousBPM > 0.0, "Invalid previous tempo, it should be positive");
    ED_ASSERTSTR(newBPM > 0.0, "Invalid new tempo, it should be positive");
    ED_ASSERTSTR(newBPM != previousBPM, "Invalid new tempo, it should different from the previous tempo");

    setText(QObject::tr("Set Tempo to %1").arg(newBPM, 0, 'f', 2));
}

//----------------------------------------------------------------------------------------

void TimelineSetBPMUndoCommand::undo()
{
    Editor::GetInstance().GetTimelineDockWidget()->SetBeatsPerMinute(mPreviousBPM);
}

//----------------------------------------------------------------------------------------

void TimelineSetBPMUndoCommand::redo()
{
    Editor::GetInstance().GetTimelineDockWidget()->SetBeatsPerMinute(mNewBPM);
}

//----------------------------------------------------------------------------------------

TimelineSetBlockPositionUndoCommand::TimelineSetBlockPositionUndoCommand(TimelineBlockGraphicsItem * blockItem,
                                                                         bool hasLaneChanged,
                                                                         unsigned int newLane,
                                                                         Pegasus::Timeline::Beat newBeat,
                                                                         unsigned int mouseClickID,
                                                                         QUndoCommand * parent)
:   QUndoCommand(parent),
    mBlockItem(blockItem),
    mHasLaneChanged(hasLaneChanged),
    mNewLane(newLane),
    mNewBeat(newBeat),
    mMouseClickID(mouseClickID)
{
    if (blockItem != nullptr)
    {
        mPreviousLane = blockItem->GetLane();
        mPreviousBeat = blockItem->GetBeat();
    }
    else
    {
        ED_FAILSTR("Invalid block given to the undo command");
    }

    SetText();
}

//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::undo()
{
    Pegasus::Timeline::IBlockProxy * const blockProxy = mBlockItem->GetBlockProxy();
    ED_ASSERT(blockProxy != nullptr);
    Pegasus::Timeline::ILaneProxy * const newLaneProxy = blockProxy->GetLane();
    ED_ASSERT(newLaneProxy != nullptr);
    Pegasus::Timeline::ITimelineProxy * const timelineProxy = newLaneProxy->GetTimeline();
    ED_ASSERT(timelineProxy != nullptr);

    if (mHasLaneChanged)
    {
        Pegasus::Timeline::ILaneProxy * const previousLaneProxy = timelineProxy->GetLane(mPreviousLane);
        ED_ASSERT(previousLaneProxy != nullptr);

        newLaneProxy->MoveBlockToLane(blockProxy, previousLaneProxy, mPreviousBeat);
    }
    else
    {
        newLaneProxy->SetBlockBeat(blockProxy, mPreviousBeat);
    }

    mBlockItem->SetLane(mPreviousLane, false);
    mBlockItem->SetBeat(mPreviousBeat, true);
}

//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::redo()
{
    Pegasus::Timeline::IBlockProxy * const blockProxy = mBlockItem->GetBlockProxy();
    ED_ASSERT(blockProxy != nullptr);
    Pegasus::Timeline::ILaneProxy * const previousLaneProxy = blockProxy->GetLane();
    ED_ASSERT(previousLaneProxy != nullptr);
    Pegasus::Timeline::ITimelineProxy * const timelineProxy = previousLaneProxy->GetTimeline();
    ED_ASSERT(timelineProxy != nullptr);

    if (mHasLaneChanged)
    {
        Pegasus::Timeline::ILaneProxy * const newLaneProxy = timelineProxy->GetLane(mNewLane);
        ED_ASSERT(newLaneProxy != nullptr);

        previousLaneProxy->MoveBlockToLane(blockProxy, newLaneProxy, mNewBeat);
    }
    else
    {
        previousLaneProxy->SetBlockBeat(blockProxy, mNewBeat);
    }

    mBlockItem->SetLane(mNewLane, false);
    mBlockItem->SetBeat(mNewBeat, true);
}

//----------------------------------------------------------------------------------------

int TimelineSetBlockPositionUndoCommand::id() const
{
    // Create a unique ID by combining:
    // - a 24 bit ID for the moved block
    // - an 8 bit ID for the mouse click, changing each time the mouse click is released
    return ((mBlockItem->GetBlockID() & 0x00FFFFFF) << 8) | (mMouseClickID & 0x000000FF);
}

//----------------------------------------------------------------------------------------

bool TimelineSetBlockPositionUndoCommand::mergeWith(const QUndoCommand * command)
{
    const TimelineSetBlockPositionUndoCommand * newCommand = static_cast<const TimelineSetBlockPositionUndoCommand *>(command);
    
    if (newCommand->mBlockItem != mBlockItem)
    {
        return false;
    }

    mHasLaneChanged = (newCommand->mNewLane != mPreviousLane);
    mNewLane = newCommand->mNewLane;
    mNewBeat = newCommand->mNewBeat;

    SetText();
    return true;
}

//----------------------------------------------------------------------------------------

void TimelineSetBlockPositionUndoCommand::SetText()
{
    if (mHasLaneChanged)
    {
        setText(QObject::tr("Move Block \"%1\" to Lane %2 and Beat %3")
                        .arg(mBlockItem->GetBlockProxy()->GetEditorString()).arg(mNewLane).arg(mNewBeat));
    }
    else
    {
        setText(QObject::tr("Move Block \"%1\" to Beat %2")
                        .arg(mBlockItem->GetBlockProxy()->GetEditorString()).arg(mNewBeat));
    }
}

//----------------------------------------------------------------------------------------

TimelineSetMultiBlockPositionUndoCommand::TimelineSetMultiBlockPositionUndoCommand(TimelineBlockGraphicsItem * blockItem,
                                                                                   const QList<QGraphicsItem *> & blockItemList,
                                                                                   bool hasLaneChanged,
                                                                                   unsigned int newLane,
                                                                                   Pegasus::Timeline::Beat newBeat,
                                                                                   unsigned int mouseClickID,
                                                                                   QUndoCommand * parent)
:   QUndoCommand(parent)
{
    ED_ASSERTSTR(blockItemList.count() >= 2, "Invalid selection size (%d), it must be >= 2", blockItemList.count());
    ED_ASSERTSTR(blockItem != nullptr, "Invalid block given to the undo command");
    ED_ASSERTSTR(blockItemList.contains(blockItem), "Invalid selection, it must contain the current block");

    mID = 0;

    // Create a child undo command for each selected block
    foreach (QGraphicsItem * const selectedItem, blockItemList)
    {
        TimelineBlockGraphicsItem * const selectedBlockItem = static_cast<TimelineBlockGraphicsItem * const>(selectedItem);

        if (selectedBlockItem == blockItem)
        {
            // If the selected block is the one passed to this undo command,
            // create the undo command with the move parameters
            mUndoCommands.push_back(new TimelineSetBlockPositionUndoCommand(selectedBlockItem,
                                                                            hasLaneChanged, newLane, newBeat,
                                                                            0, this));
        }
        else
        {
            // If the selected block is not the one passed to this undo command,
            // create an undo command with no move parameter
            mUndoCommands.push_back(new TimelineSetBlockPositionUndoCommand(selectedBlockItem,
                                                                            false,
                                                                            selectedBlockItem->GetLane(),
                                                                            selectedBlockItem->GetBeat(),
                                                                            0, this));
        }

        // Retain the high 24 bits of the ID of each undo command and add them together
        // (it is OK to overflow, we just want a unique ID)
        mID += (mUndoCommands.back()->id() & 0xFFFFFF00);
    }

    // Add the mouse ID to the command ID (lower 8 bits)
    mID = (mID & 0xFFFFFF00) | (mouseClickID & 0x000000FF);

    setText(QObject::tr("Move %1 Blocks").arg(blockItemList.count()));
}

//----------------------------------------------------------------------------------------

int TimelineSetMultiBlockPositionUndoCommand::id() const
{
    return mID;
}

//----------------------------------------------------------------------------------------

bool TimelineSetMultiBlockPositionUndoCommand::mergeWith(const QUndoCommand * command)
{
    const TimelineSetMultiBlockPositionUndoCommand * newCommand = static_cast<const TimelineSetMultiBlockPositionUndoCommand *>(command);
    bool mergeOccurred = false;

    // Merge each child command of the incoming command
    foreach (QUndoCommand * const newBlockUndoCommand, newCommand->mUndoCommands)
    {
        TimelineSetBlockPositionUndoCommand * const newBlockCommand = static_cast<TimelineSetBlockPositionUndoCommand * const>(newBlockUndoCommand);
        TimelineBlockGraphicsItem * const newBlockItem = newBlockCommand->GetBlockItem();
        foreach (QUndoCommand * const currentBlockUndoCommand, mUndoCommands)
        {
            TimelineSetBlockPositionUndoCommand * const currentBlockCommand = static_cast<TimelineSetBlockPositionUndoCommand * const>(currentBlockUndoCommand);
            TimelineBlockGraphicsItem * const currentBlockItem = currentBlockCommand->GetBlockItem();
            if (newBlockItem == currentBlockItem)
            {
                mergeOccurred |= currentBlockCommand->mergeWith(newBlockCommand);
                break;
            }
        }
    }

    return mergeOccurred;
}
