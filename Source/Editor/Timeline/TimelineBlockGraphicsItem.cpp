#include "TimelineBlockGraphicsItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>


//! Default length in pixels for one time unit (for a zoom of 1.0f)
//! \todo Move this value in a more general area
#define TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH        128.0f

//! Default height in pixels for a timeline lane (for a zoom of 1.0f)
//! \todo Move this value in a more general area
#define TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT            32.0f

//! Depth of the block when drawn. Positive since it needs to be rendered in front of the grid at least
#define TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE                10.0f

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::TimelineBlockGraphicsItem(float position, float length)
:   QGraphicsItem()
{
    SetPosition(position);
    SetLength(length);

    // Make the block movable and selectable with the mouse
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the block movements
    setFlag(ItemSendsGeometryChanges);

    // Caching performed at paint device level, best quality and lower memory usage
    //! \todo Check the caching mode once scaling is applied to the block
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

void TimelineBlockGraphicsItem::SetPosition(float pos)
{
    if (pos < 0.0f)
    {
        //! \todo Assert for invalid position
        mPosition = 0.0f;
    }
    else
    {
        mPosition = pos;
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLength(float length)
{
    if (length <= 0.0f)
    {
        //! \todo Assert for invalid position
        mLength = 1.0f;
    }
    else
    {
        mLength = length;
    }
}

//----------------------------------------------------------------------------------------

QRectF TimelineBlockGraphicsItem::boundingRect() const
{
    //! \todo Handle the constants and the shadow size properly
    const float adjust = 2.0f;
    return QRectF(0.0f - adjust,
                  0.0f - adjust,
                  mLength * TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH + 3.0f + adjust,
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
                 mLength * TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH,
                 TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT);
    return path;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::black);
    painter->setBrush(Qt::darkGray);

    painter->drawRect(0.0f,
                      0.0f,
                      mLength * TIMELINE_BLOCK_GRAPHICS_ITEM_TIME_UNIT_WIDTH,
                      TIMELINE_BLOCK_GRAPHICS_ITEM_LANE_HEIGHT);

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
    switch (change) {
    case ItemPositionHasChanged:
        //foreach (Edge *edge, edgeList)
        //    edge->adjust();
        //graph->itemMoved();
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