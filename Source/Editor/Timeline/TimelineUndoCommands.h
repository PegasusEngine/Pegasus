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

#include "Pegasus/Timeline/Shared/TimelineDefs.h"

class TimelineBlockGraphicsItem;


//! Undo command for setting the beats per minute of the timeline
class TimelineSetBPMUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param previousBPM Tempo in beats per minute before the command
    //! \param newBPM Tempo in beats per minute after the command
    //! \param parent Optional parent command
    TimelineSetBPMUndoCommand(double previousBPM, double newBPM,
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
                                        bool hasLaneChanged,
                                        unsigned int newLane,
                                        Pegasus::Timeline::Beat newBeat,
                                        unsigned int mouseClickID,
                                        QUndoCommand * parent = nullptr);

    //! Get the graphics item of the block to move
    //! \return Graphics item of the block to move (!= nullptr)
    TimelineBlockGraphicsItem * GetBlockItem() const { return mBlockItem; }

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

    //! Set the text of the command from the current state
    void SetText();

    //! Graphics item of the block to move (!= nullptr)
    TimelineBlockGraphicsItem * mBlockItem;

    //! True if the lane has changed, otherwise only beat is used
    bool mHasLaneChanged;

    //! Index of the previous lane of the block (>= 0)
    unsigned int mPreviousLane;

    //! Index of the new lane of the block (>= 0)
    unsigned int mNewLane;

    //! Previous position of the block, measured in ticks
    Pegasus::Timeline::Beat mPreviousBeat;

    //! New position of the block, measured in ticks
    Pegasus::Timeline::Beat mNewBeat;

    //! Mouse click ID, unique for each time the click is maintained while moving a block.
    //! Used to create new undo commands each time the mouse click is released.
    unsigned int mMouseClickID;
};

//----------------------------------------------------------------------------------------

//! Undo command for setting the position of multiple blocks
// (parent of a list of \a TimelineSetBlockPositionUndoCommand, one per selected block)
class TimelineSetMultiBlockPositionUndoCommand : public QUndoCommand
{
public:

    //! Constructor
    //! \param blockItem Graphics item of the block to move (!= nullptr)
    //! \param blockItemList List of block graphics items of the current selection
    //! \param hasLaneChanged True if \a newLane is used, otherwise only beat is used
    //! \param newLane Index of the new lane of the block (>= 0)
    //! \param newBeat New position of the block, measured in ticks
    //! \param mouseClickID Mouse click ID, unique for each time the click
    //!                     is maintained while moving a block. Used to create new undo commands
    //!                     each time the mouse click is released.
    //! \param parent Optional parent command
    TimelineSetMultiBlockPositionUndoCommand(TimelineBlockGraphicsItem * blockItem,
                                             const QList<QGraphicsItem *> & blockItemList,
                                             bool hasLaneChanged,
                                             unsigned int newLane,
                                             Pegasus::Timeline::Beat newBeat,
                                             unsigned int mouseClickID,
                                             QUndoCommand * parent = nullptr);

    // undo() and redo() have their default implementations, which executes those functions
    // for the list of children commands

    //! Return an ID to uniquely identify this command. Used to enable \a mergeWith()
    //! \return Unique ID derived from the block ID of each child command and the mouse click ID
    virtual int id() const;

    //! Merge function to combine the new undo command with the current one if possible
    //! \param command Incoming command
    //! \return False if the commands cannot be merged (accessing a different block for example),
    //!         true if succeeded
    virtual bool mergeWith(const QUndoCommand * command);

private:

    //! Set the text of the command from the current state
    void SetText();

    //! Child undo commands, one per selected block
    QList<QUndoCommand *> mUndoCommands;

    //! Command ID (returned by \a id()) computed from the ID of the child undo commands
    int mID;
};


#endif  // EDITOR_TIMELINEUNDOCOMMANDS_H
