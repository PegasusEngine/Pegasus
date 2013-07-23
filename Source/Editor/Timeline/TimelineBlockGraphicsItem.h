/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockGraphicsItem.h
//! \author	Kevin Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#ifndef EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
#define EDITOR_TIMELINEBLOCKGRAPHICSITEM_H

#include <QGraphicsItem>


//! Graphics item representing one timeline block.
//! Contains the block rendering properties and the rendering code.
class TimelineBlockGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor
    //! \param lane Index of the timeline lane the block belongs to
    //! \param basePosition Based position of the block, in time units (1.0f for one beat, >= 0.0f)
    //! \param baseLength Base length of the block, in time units (1.0f for one beat, > 0.0f)
    //! \param baseColor Based color of the block, when not selected
    //! \param horizontalScale Horizontal scale of the block, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed block, > 1.0f for an expanded block
    TimelineBlockGraphicsItem(unsigned int lane,
                              float basePosition,
                              float baseLength,
                              const QColor & baseColor,
                              float horizontalScale);

    //! Destructor
    ~TimelineBlockGraphicsItem();

    //! Set the timeline lane of the block
    //! \param lane Index of the timeline lane the block belongs to
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetLane(unsigned int lane, bool updateItem = true);

    //! Set the base position of the block
    //! \param basePosition Base position of the block, in time units (1.0f for one beat, >= 0.0f)
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetBasePosition(float basePosition, bool updateItem = true);

    //! Set the base length of the block
    //! \param baseLength Base length of the block, in time units (1.0f for one beat, > 0.0f)
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetBaseLength(float baseLength, bool updateItem = true);

    //! Set the horizontal scale of the block
    //! \param scale 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    void SetHorizontalScale(float scale);

    //! Get the horizontal scale of the block
    //! \return 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    //inline float GetHorizontalScale() const { return mHorizontalScale; }


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 10 };
    int type() const { return Type; }


protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    //! Update the vertical position from the lane index
    void SetLanePositionFromLane();

    //! Update the scaled position from the base position
    void SetPositionFromBasePosition();

    //! Update the scaled length from the base length
    void SetLengthFromBaseLength();

    //! Set the vertical position (linked to lane size) from an input vertical coordinate
    //! \param lanePosition Vertical position in pixels (for a zoom of 1.0f)
    void SetLaneFromPosition(float lanePosition);

    //! Update the base position from the scaled position
    void SetBasePositionFromPosition();

    //! Update the base length from the base scaled length
    void SetBaseLengthFromLength();


    //! Index of the timeline lane the block belongs to
    unsigned int mLane;
    
    //! Base position of the block in the lane, in time units (1.0f for one beat, >= 0.0f)
    float mBasePosition;

    //! Base length of the block, in time units (1.0f for one beat, > 0.0f)
    float mBaseLength;

    //! Base color of the block, when not selected
    QColor mBaseColor;

    //! Horizontal scale of the block
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Vertical position in pixels (with no zoom applied)
    float mLanePosition;

    //! Position in pixels (with no zoom applied, but horizontal scale applied)
    float mPosition;

    //! Length in pixels (with no zoom applied, but horizontal scale applied)
    float mLength;
};


#endif  // EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
