#ifndef TIMELINEBLOCKGRAPHICSITEM_H
#define TIMELINEBLOCKGRAPHICSITEM_H

#include <QGraphicsItem>

//! \class TimelineBlockGraphicsItem
//! Graphics item representing one timeline block.
//! Contains the block rendering properties and the rendering code.
class TimelineBlockGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor
    //! \param position Initial position of the block, in time units (1.0f for one beat, >= 0.0f)
    //! \param length Initial length of the block, in time units (1.0f for one beat, > 0.0f)
    TimelineBlockGraphicsItem(float position, float length);

    //! Destructor
    ~TimelineBlockGraphicsItem();

    //! Set the position of the block
    //! \param pos Position of the block, in time units (1.0f for one beat, >= 0.0f)
    void SetPosition(float pos);

    //! Set the length of the block
    //! \param length Length of the block, in time units (1.0f for one beat, > 0.0f)
    void SetLength(float length);

    //! \todo Document those functions

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    //! Position of the block in the lane, in time units (1.0f for one beat, >= 0.0f)
    float mPosition;
    
    //! Length of the block, in time units (1.0f for one beat, > 0.0f)
    float mLength;

    QPointF newPos;
    //GraphWidget *graph;
};

#endif // TIMELINEBLOCKGRAPHICSITEM_H
