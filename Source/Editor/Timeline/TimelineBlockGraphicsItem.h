/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#ifndef EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
#define EDITOR_TIMELINEBLOCKGRAPHICSITEM_H

#include <QGraphicsObject>

#include "Pegasus/Timeline/Shared/TimelineDefs.h"

namespace Pegasus {
    namespace Timeline {
        class IBlockProxy;
    }
}

class QUndoStack;


//! Graphics item representing one timeline block.
//! Contains the block rendering properties and the rendering code.
class TimelineBlockGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param blockProxy Block proxy associated with the item
    //! \param lane Index of the timeline lane the block belongs to
    //! \param horizontalScale Horizontal scale of the block, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed block, > 1.0f for an expanded block
    TimelineBlockGraphicsItem(Pegasus::Timeline::IBlockProxy * blockProxy,
                              unsigned int lane,
                              float horizontalScale,
                              QUndoStack* undoStack);

    //! Destructor
    virtual ~TimelineBlockGraphicsItem();

    //! Get the block proxy associated with the item
    //! \return Block proxy associated with the item (!= nullptr)
    inline Pegasus::Timeline::IBlockProxy * GetBlockProxy() const { return mBlockProxy; }


    //! Set the timeline lane of the block
    //! \param lane Index of the timeline lane the block belongs to
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetLane(unsigned int lane, bool updateItem = true);

    //! Get the timeline lane of the block
    //! \return Index of the timeline lane the block belongs to
    inline unsigned int GetLane() const { return mLane; }

    //! Set the position of the block
    //! \param beat Position of the block, measured in ticks
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetBeat(Pegasus::Timeline::Beat beat, bool updateItem = true);

    //! Get the position of the block
    //! \return Position of the block, measured in ticks
    inline Pegasus::Timeline::Beat GetBeat() const { return mBeat; }

    //! Set the duration of the block
    //! \param duration Duration of the block, measured in ticks (> 0)
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetDuration(Pegasus::Timeline::Duration duration, bool updateItem = true);

    //! Get the duration of the block
    //! \return Duration of the block, measured in ticks (> 0)
    inline Pegasus::Timeline::Duration GetDuration() const { return mDuration; }

    //! Set the horizontal scale of the block
    //! \param scale 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    void SetHorizontalScale(float scale);

    //! Get the horizontal scale of the block
    //! \return 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    //inline float GetHorizontalScale() const { return mHorizontalScale; }

    //! Get the block ID, unique to each block, used for merging undo commands
    //! (so moving a block does not create a new undo command for each pixel moved)
    //! \return ID unique to the block
    inline unsigned int GetBlockID() const { return mBlockID; }


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 10 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the block has been moved by the user
    void BlockMoved();

    //! Emitted when the user does a double click to this block
    void DoubleClicked(Pegasus::Timeline::IBlockProxy * block);

    //------------------------------------------------------------------------------------
    
protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //------------------------------------------------------------------------------------

private:

    //! Update the scaled position in pixels from the position in ticks
    void SetXFromBeat();

    //! Get the beat in ticks from the scaled position
    //! \param x Horizontal position in pixels (for a zoom of 1.0f)
    //! \return Beat, measured in ticks
    Pegasus::Timeline::Beat GetBeatFromX(float x) const;

    //! Update the vertical position from the lane index
    void SetYFromLane();

    //! Get the lane index (linked to lane size) from an input vertical coordinate
    //! \param lanePosition Vertical position in pixels (for a zoom of 1.0f)
    //! \return Lane index
    unsigned int GetLaneFromY(float lanePosition) const;

    //! Update the scaled length from the duration in ticks
    void SetLengthFromDuration();

    //! Update the duration in ticks from the scaled length
    void SetBaseLengthFromPixelLength();


    //! Block proxy associated with the item
    Pegasus::Timeline::IBlockProxy * mBlockProxy;

    //! Reference to its undo stack
    QUndoStack* mUndoStack;

    //! Index of the timeline lane the block belongs to
    unsigned int mLane;
    
    //! Name of the block, displayed as text on top of the block
    QString mName;

    //! Base color of the block, when not selected
    QColor mBaseColor;

    //! Position of the block in the lane, measured in ticks
    Pegasus::Timeline::Beat mBeat;

    //! Duration of the block, measured in ticks (> 0)
    Pegasus::Timeline::Duration mDuration;

    //! Horizontal scale of the block
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Position in pixels (with no zoom applied, but horizontal scale applied)
    float mX;

    //! Vertical position in pixels (with no zoom applied)
    float mY;

    //! Length in pixels (with no zoom applied, but horizontal scale applied)
    float mLength;

    //! True if undo commands can be sent
    bool mEnableUndo;

    //! Block ID counter, assigned uniquely to each block, used for merging undo commands
    //! (so moving a block does not create a new undo command for each pixel moved)
    static unsigned int sCurrentBlockID;
    
    //! Block ID, unique to each block, used for merging undo commands
    //! (so moving a block does not create a new undo command for each pixel moved)
    unsigned int mBlockID;

    //! Mouse click ID, unique for each time the click is maintained while moving a block.
    //! Used to create new undo commands each time the mouse click is released.
    static unsigned int sMouseClickID;
};


#endif  // EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
