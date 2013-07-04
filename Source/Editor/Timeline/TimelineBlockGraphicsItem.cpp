/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineGraphicsItem.cpp
//! \author	Kevin Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#include "Timeline/TimelineBlockGraphicsItem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>


//! Default length in pixels for one time unit (for a zoom of 1.0f)
//! \todo Move this value in a more general area
#define TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH        128.0f

//! Height in pixels for a timeline lane (for a zoom of 1.0f)
//! \todo Move this value in a more general area
#define TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT            24.0f

//! Height in pixels of the space between timeline blocks and the edge of a lane (for a zoom of 1.0f)
//! \todo Move this value in a more general area
#define TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_SEPARATOR_HEIGHT  3.0f
    
//! Depth of the block when drawn. Positive since it needs to be rendered in front of the grid at least
#define TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE                10.0f

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::TimelineBlockGraphicsItem(unsigned int lane,
                                                     float basePosition,
                                                     float baseLength,
                                                     const QColor & baseColor,
                                                     float horizontalScale)
:   QGraphicsItem(),
    mBaseColor(baseColor)
{
    if (horizontalScale <= 0.0f)
    {
        //! \todo Assert for invalid scale
        mHorizontalScale = 1.0f;
    }
    else
    {
        mHorizontalScale = horizontalScale;
    }

    // Set the initial position and length, and update the scaled position and length
    SetBaseLength(baseLength, false);
    SetLane(lane, false);
    SetBasePosition(basePosition, true);

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
    SetLanePositionFromLane();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mPosition, mLanePosition);
        update(boundingRect());
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBasePosition(float basePosition, bool updateItem)
{
    if (basePosition < 0.0f)
    {
        //! \todo Assert for invalid position
        mBasePosition = 0.0f;
    }
    else
    {
        mBasePosition = basePosition;
    }

    // Update the scaled position
    SetPositionFromBasePosition();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mPosition, mLanePosition);
        update(boundingRect());
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBaseLength(float baseLength, bool updateItem)
{
    if (baseLength <= 0.0f)
    {
        //! \todo Assert for invalid position
        mBaseLength = 1.0f;
    }
    else
    {
        mBaseLength = baseLength;
    }

    // Update the scaled length
    SetLengthFromBaseLength();

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
    SetPositionFromBasePosition();
    SetLengthFromBaseLength();

    // Set the new position of the graphics item
    setPos(mPosition, mLanePosition);

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

QRectF TimelineBlockGraphicsItem::boundingRect() const
{
    //! \todo Handle the constants and the shadow size properly
    const float adjust = 2.0f;
    return QRectF(0.0f - adjust,
                  0.0f - adjust,
                  mLength + adjust,
                  TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT + 3.0f + adjust);
}

//----------------------------------------------------------------------------------------

QPainterPath TimelineBlockGraphicsItem::shape() const
{
    // By default, returns boundingRect().
    // This overload allows to not take the shadow into account by the mouse cursor
    // and for collision detection.

    QPainterPath path;
    path.addRect(0.0f,
                 0.0f,
                 mLength,
                 TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT);
    return path;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? mBaseColor.darker(150) : mBaseColor;
    if (option->state & QStyle::State_MouseOver)
    {
        fillColor = fillColor.lighter(125);
    }

    QColor outlineColor = mBaseColor.darker(200);

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());


    painter->setPen(outlineColor);
    painter->setBrush(fillColor);

    painter->drawRect(0.0f,
                      0.0f,
                      mLength,
                      TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT);

    QFont font = painter->font();
    //font.setBold(true);
    const int fontPixelSize = 10;
    font.setPixelSize(fontPixelSize);
    painter->setFont(font);
    painter->setPen(Qt::black);
    const float textMargin = (TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT - (float)fontPixelSize) * 0.5f;
    QRectF textRect(textMargin,
                    textMargin,
                    mLength - 2.0f * textMargin,
                    TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT - 2.0f * textMargin);
    painter->drawText(textRect, QString("%1").arg(lod));
    
    /*    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);*/
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

                // Clamp the blank to the bounds of the scene
                QRectF rect = scene()->sceneRect();
                if (!rect.contains(newPos))
                {
                    newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
                    newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
                    positionAffected = true;
                }
                mPosition = newPos.x();
                SetBasePositionFromPosition();

                // If the lane changes, update the coordinates accordingly
                SetLaneFromPosition(newPos.y());
                SetLanePositionFromLane();
                if (mLanePosition != newPos.y())
                {
                    newPos.setY(mLanePosition);
                    positionAffected = true;
                }

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
    
void TimelineBlockGraphicsItem::SetLanePositionFromLane()
{
    mLanePosition = TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_SEPARATOR_HEIGHT +
                        mLane * (TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT + 2.0f * TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_SEPARATOR_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetPositionFromBasePosition()
{
    mPosition = mBasePosition * mHorizontalScale;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLengthFromBaseLength()
{
    mLength = mBaseLength * (mHorizontalScale * TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLaneFromPosition(float lanePosition)
{
    int lane = static_cast<int>(floor((lanePosition - TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_SEPARATOR_HEIGHT)
                    / (TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT + 2.0f * TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_SEPARATOR_HEIGHT)));
    if (lane < 0)
    {
        lane = 0;
    }

    mLane = static_cast<unsigned int>(lane);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBasePositionFromPosition()
{
    mBasePosition = mPosition / mHorizontalScale;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBaseLengthFromLength()
{
    mBaseLength = mLength / (mHorizontalScale * TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH);
}
