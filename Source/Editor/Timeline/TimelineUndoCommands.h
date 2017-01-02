/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineUndoCommands.h
//! \author	Karolyn Boulanger
//! \date	20th January 2014
//! \brief	List of timeline commands that can be undone

#ifndef EDITOR_TIMELINEUNDOCOMMANDS_H
#define EDITOR_TIMELINEUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QMap>
#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "MessageControllers/MsgDefines.h"

class TimelineBlockGraphicsItem;
class TimelineDockWidget;


//! Undo command for setting the beats per minute of the timeline
class TimelineSetBPMUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param previousBPM Tempo in beats per minute before the command
    //! \param newBPM Tempo in beats per minute after the command
    //! \param parent Optional parent command
    TimelineSetBPMUndoCommand(double previousBPM, double newBPM, TimelineDockWidget* messenger,
                              QUndoCommand * parent = nullptr);

    //! Undo function, restores the previous BPM
    virtual void undo();

    //! Redo function, applies the new BPM
    virtual void redo();

private:

    //! Tempo in beats per minute before the command
    double mPreviousBPM;

    //! Tempo in beats per minute after the command
    double mNewBPM;

    TimelineDockWidget* mMessenger;
};

//----------------------------------------------------------------------------------------

//! Undo command for setting the position of a block
class TimelineSetBlockPositionUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param blockItem Graphics item of the block to move (!= nullptr)
    //! \param hasLaneChanged True if \a newLane is used, otherwise only beat is used
    //! \param newLane Index of the new lane of the block (>= 0)
    //! \param newBeat New position of the block, measured in ticks
    //! \param mouseClickID Mouse click ID, unique for each time the click
    //!                     is maintained while moving a block. Used to create new undo commands
    //!                     each time the mouse click is released.
    //! \param parent Optional parent command
    TimelineSetBlockPositionUndoCommand(TimelineBlockGraphicsItem * blockItem,
                                        unsigned int newLane,
                                        Pegasus::Timeline::Beat newBeat,
                                        unsigned int mouseClickID,
                                        TimelineDockWidget* messenger,
                                        QUndoCommand * parent = nullptr);

    //! Undo function, restores the previous position
    virtual void undo();

    //! Redo function, applies the new position
    virtual void redo();

    //! Return an ID to uniquely identify this command. Used to enable \a mergeWith()
    //! \return Unique ID derived from the block ID and the mouse click ID
    virtual int id() const;

    //! Merge function to combine the new undo command with the current one if possible
    //! \param command Incoming command
    //! \return False if the commands cannot be merged (accessing a different block for example),
    //!         true if succeeded
    virtual bool mergeWith(const QUndoCommand * command);

private:

    const TimelineIOMCMessage* FindMessage(bool isUndoStack, unsigned blockId) const;

    void ExecuteMessages(bool isUndoStack);

    //! Set the text of the command from the current state
    void SetText();

    typedef QMap<unsigned, TimelineIOMCMessage> BlockMessageMap;
    BlockMessageMap mRedoMessages;
    BlockMessageMap mUndoMessages;

    //! Mouse click ID, unique for each time the click is maintained while moving a block.
    //! Used to create new undo commands each time the mouse click is released.
    unsigned int mMouseClickID;

    //! ui control that will actually send the message to rendering.
    TimelineDockWidget* mMessenger;
};

//! Undo command for creating a timeline block
class TimelineCreateBlockUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param timeline handle to use.
    //! \param newClassName - class name of the block to delete/ create
    //! \param targetBlockGuid - this would be the block guid that was used if the new block was created. This handle is computed through another message.
    //! \param targetLane - initial lane to place block
    //! \param initialBeatPosition - the initial beat position to place this block
    //! \param initialBlockLength - the initial length of the current beat
    //!                     each time the mouse click is released.
    //! \param parent Optional parent command
    TimelineCreateBlockUndoCommand(
                                   const AssetInstanceHandle& timelineHandle,
                                   unsigned int targetBlockGuid,
                                   TimelineDockWidget* messenger,
                                   const QVariant& newBlockArgs,
                                   QUndoCommand * parent = nullptr);

    //! Undo function, restores the previous position
    virtual void undo();

    //! Redo function, applies the new position
    virtual void redo();

private:
    //! ui control that will actually send the message to rendering.
    TimelineDockWidget* mMessenger;
    TimelineIOMCMessage mCreateMessage;
    TimelineIOMCMessage mDeleteMessage;
};
//! Undo command for  deleting a timeline block
class TimelineDeleteBlockUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param timeline handle to use.
    //! \param parent shadowBlockStateList - list of blocks to delete
    //! \param parent messenger - messenger to use
    TimelineDeleteBlockUndoCommand(
                                   const AssetInstanceHandle& timelineHandle,
                                   const QVector<const ShadowBlockState*>& shadowBlockStates,
                                   const QVector<int>& targetLanes,
                                   const QVariantList& blockJsonStates,
                                   TimelineDockWidget* messenger,
                                   QUndoCommand * parent = nullptr);

    //! Undo function, restores the previous position
    virtual void undo();

    //! Redo function, applies the new position
    virtual void redo();

private:
    //! ui control that will actually send the message to rendering.
    TimelineDockWidget* mMessenger;
    TimelineIOMCMessage mDeleteMessage;
    QVector<TimelineIOMCMessage> mRecreateMessages;
};

#endif  // EDITOR_TIMELINEUNDOCOMMANDS_H
