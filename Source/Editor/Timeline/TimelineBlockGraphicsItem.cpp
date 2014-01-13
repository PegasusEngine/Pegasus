/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockGraphicsItem.cpp
//! \author	Kevin Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#include "Timeline/TimelineBlockGraphicsItem.h"
#include "Timeline/TimelineSizes.h"

#include "Pegasus/Timeline/Shared/IBlockProxy.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>


//! Depth of the block when drawn. Positive since it needs to be rendered in front of the grid at least
#define TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE                10.0f

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::TimelineBlockGraphicsItem(Pegasus::Timeline::IBlockProxy * blockProxy,
                                                     unsigned int lane,
                                                     float horizontalScale)
:   QGraphicsObject(),
    mBlockProxy(blockProxy),
    mName((blockProxy != nullptr) ? blockProxy->GetEditorString() : "")
{
    ED_ASSERTSTR(blockProxy != nullptr, "A block proxy must be associated with the block item");
    ED_ASSERTSTR(!QString(blockProxy->GetEditorString()).isEmpty(), "The name of the block must be defined");

    if (horizontalScale <= 0.0f)
    {
        ED_FAILSTR("Invalid horizontal scale (%f) for the timeline graphics item. It should be > 0.0f.", horizontalScale);
        mHorizontalScale = 1.0f;
    }
    else
    {
        mHorizontalScale = horizontalScale;
    }

    // Set the block color
    unsigned char red, green, blue;
    blockProxy->GetColor(red, green, blue);
    mBaseColor = QColor(red, green, blue);

    // Set the initial position and length, and update the scaled position and length
    SetBaseLength(blockProxy->GetLength(), false);
    SetLane(lane, false);
    SetBasePosition(blockProxy->GetPosition(), true);

    // Make the block movable and selectable with the mouse
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the block movements
    setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the block (positive since it needs to be rendered
    // in front of the grid at least)
    setZValue(TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE);
}

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::~TimelineBlockGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLane(unsigned int lane, bool updateItem)
{
    mLane = lane;

    // Update the vertical position
    SetYFromLane();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mX, mY);
        update(boundingRect());
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBasePosition(float basePosition, bool updateItem)
{
    if (basePosition < 0.0f)
    {
        ED_FAILSTR("Invalid base position (%f) for the timeline graphics item. It should be >= 0.0f.", basePosition);
        mBasePosition = 0.0f;
    }
    else
    {
        mBasePosition = basePosition;
    }

    // Update the scaled position
    SetXFromBasePosition();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mX, mY);
        update(boundingRect());
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBaseLength(float baseLength, bool updateItem)
{
    if (baseLength <= 0.0f)
    {
        ED_FAILSTR("Invalid base length (%f) for the timeline graphics item. It should be > 0.0f.", baseLength);
        mBaseLength = 1.0f;
    }
    else
    {
        mBaseLength = baseLength;
    }

    // Update the scaled length
    SetPixelLengthFromBaseLength();

    // Update the graphics item, so it is redrawn with the right length
    if (updateItem)
    {
        update(boundingRect());
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetHorizontalScale(float scale)
{
    // No assertion for invalid scale (performance issue).
    // The caller performs the assertion test.
    mHorizontalScale = scale;

    //! Update the scaled position and length
    SetXFromBasePosition();
    SetPixelLengthFromBaseLength();

    // Set the new position of the graphics item
    setPos(mX, mY);

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

QRectF TimelineBlockGraphicsItem::boundingRect() const
{
    return QRectF(0.0f,
                  0.0f,
                  mPixelLength,
                  TIMELINE_BLOCK_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
    Q_UNUSED(widget);

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    // Set the outline of the block to a solid line with width 1 for any zoom (cosmetic pen) and square corners.
    // If the LOD is too low, remove the outline, otherwise the rendering is messy and aliased
    if (lod > 0.5f)
    {
        QColor outlineColor = mBaseColor.darker(200);
        QPen outlinePen(QBrush(outlineColor), 0, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
        painter->setPen(outlinePen);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    // Set the fill color, darker when selected, lighter when hovered by the mouse
    QColor fillColor = (option->state & QStyle::State_Selected) ? mBaseColor.darker(150) : mBaseColor;
    if (option->state & QStyle::State_MouseOver)
    {
        fillColor = fillColor.lighter(125);
    }
    painter->setBrush(fillColor);

    // Draw the block
    painter->drawRect(0.0f,
                      0.0f,
                      mPixelLength,
                      TIMELINE_BLOCK_HEIGHT);

    // Draw the label of the block
    QFont font = painter->font();
    font.setPixelSize(TIMELINE_BLOCK_FONT_HEIGHT);
    font.setBold(false);
    painter->setFont(font);
    painter->setPen(Qt::black);
    const float fontHeightScale = 1.25f;                // To handle text below the base line
    const float textMargin = (TIMELINE_BLOCK_HEIGHT - fontHeightScale * (float)TIMELINE_BLOCK_FONT_HEIGHT) * 0.5f;
    QRectF textRect(textMargin * 2,                     // Added extra space on the left
                    textMargin,
                    mPixelLength - 3.0f * textMargin,   // Takes the extra space on the left into account
                    TIMELINE_BLOCK_HEIGHT - 2.0f * textMargin);
    painter->drawText(textRect, mName);
}

//----------------------------------------------------------------------------------------

QVariant TimelineBlockGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionChange:
            {
                QPointF newPos = value.toPointF();
                bool positionAffected = false;

                // Clamp the block to the bounds of the scene
                //! \todo Do not use the scene, use the actual lanes rectangle instead
                QRectF rect = scene()->sceneRect();
                rect.setLeft(0.0f);
                if (!rect.contains(newPos))
                {
                    newPos.setX(qMin(rect.right (), qMax(newPos.x(), rect.left())));
                    newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top ())));
                    positionAffected = true;
                }
                mX = newPos.x();
                SetBasePositionFromX();

                // If the lane changes, update the coordinates accordingly
                SetLaneFromY(newPos.y());
                SetYFromLane();
                if (mY != newPos.y())
                {
                    newPos.setY(mY);
                    positionAffected = true;
                }

                // Update the position of the Pegasus timeline block
                mBlockProxy->SetPosition(mBasePosition);

                //! \todo Add support for lane changes in Pegasus

                //! \todo Update the Pegasus linked list of the lane,
                //!       otherwise the order of the blocks cannot be changed

                // Tell parents the block has moved
                emit BlockMoved();

                // Return the new coordinates if they have been forced to change
                if (positionAffected)
                {
                    return newPos;
                }
            }
            break;

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetXFromBasePosition()
{
    mX = mBasePosition * mHorizontalScale * TIMELINE_BEAT_WIDTH;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBasePositionFromX()
{
    mBasePosition = mX / (mHorizontalScale * TIMELINE_BEAT_WIDTH);
}

//----------------------------------------------------------------------------------------
    
void TimelineBlockGraphicsItem::SetYFromLane()
{
    mY = TIMELINE_BLOCK_MARGIN_HEIGHT + mLane * TIMELINE_LANE_HEIGHT;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLaneFromY(float lanePosition)
{
    int lane = static_cast<int>(floor((lanePosition - TIMELINE_BLOCK_MARGIN_HEIGHT) / TIMELINE_LANE_HEIGHT));
    if (lane < 0)
    {
        lane = 0;
    }

    //! \todo Clamp to avoid blocks to move to an invalid lane (job000088)

    mLane = static_cast<unsigned int>(lane);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetPixelLengthFromBaseLength()
{
    mPixelLength = mBaseLength * (mHorizontalScale * TIMELINE_BEAT_WIDTH);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBaseLengthFromPixelLength()
{
    mBaseLength = mPixelLength / (mHorizontalScale * TIMELINE_BEAT_WIDTH);
}
