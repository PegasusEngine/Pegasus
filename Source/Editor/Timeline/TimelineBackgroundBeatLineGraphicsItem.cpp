/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBackgroundBeatLineGraphicsItem.cpp
//! \author	Kevin Boulanger
//! \date	23rd July 2013
//! \brief	Graphics item representing a vertical line for a beat of the background in the timeline

#include "Timeline/TimelineBackgroundBeatLineGraphicsItem.h"
#include "Timeline/TimelineSizes.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>


TimelineBackgroundBeatLineGraphicsItem::TimelineBackgroundBeatLineGraphicsItem(unsigned int beat,
                                                                               unsigned int numLanes,
                                                                               float horizontalScale)
:   QGraphicsItem(),
    mBeat(beat)
{
    if (numLanes == 0)
    {
        ED_FAILSTR("Invalid number of lanes (== 0) for the timeline graphics item.");
        mNumLanes = 1;
    }
    else
    {
        mNumLanes = numLanes;
    }

    if (horizontalScale <= 0.0f)
    {
        ED_FAILSTR("Invalid horizontal scale (<= 0.0f) for the timeline graphics item.");
        mHorizontalScale = 1.0f;
    }
    else
    {
        mHorizontalScale = horizontalScale;
    }

    // Set the initial scaled position
    SetPositionFromBeat();

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the line to be the second most in the background
    //! \todo Create TimelineDepths.h and set the values so they do not conflict
    setZValue(1.0f);
}

//----------------------------------------------------------------------------------------

TimelineBackgroundBeatLineGraphicsItem::~TimelineBackgroundBeatLineGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void TimelineBackgroundBeatLineGraphicsItem::SetNumLanes(unsigned int numLanes)
{
    if (numLanes == 0)
    {
        ED_FAILSTR("Invalid number of lanes (== 0) for the timeline graphics item.");
        mNumLanes = 1;
    }
    else
    {
        mNumLanes = numLanes;
    }

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

void TimelineBackgroundBeatLineGraphicsItem::SetHorizontalScale(float scale)
{
    // No assertion for invalid scale (performance issue).
    // The caller performs the assertion test.
    mHorizontalScale = scale;

    //! Update the scaled position
    SetPositionFromBeat();

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

QRectF TimelineBackgroundBeatLineGraphicsItem::boundingRect() const
{
    if ((mBeat % 4) == 0)
    {
        // When on a measure, use the text bounding box, ignoring the line width, as we consider
        // the text to be always larger than the line
        return QRectF(-TIMELINE_BEAT_NUMBER_BLOCK_WIDTH * 0.5f,
                      -TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT,
                      TIMELINE_BEAT_NUMBER_BLOCK_WIDTH,
                      TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT + mNumLanes * TIMELINE_LANE_HEIGHT);
    }
    else
    {
        // When not on a measure, ignore the beat text and use the line width
        return QRectF(-TIMELINE_MEASURE_LINE_WIDTH * 0.5f,
                      0.0f,
                      TIMELINE_MEASURE_LINE_WIDTH,
                      mNumLanes * TIMELINE_LANE_HEIGHT);
    }
}

//----------------------------------------------------------------------------------------

void TimelineBackgroundBeatLineGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
    Q_UNUSED(widget);

    // Set the pen parameters (depending if the line is a measure or an intermediate beat)
    //! \todo Handle measures of less or more than 4 beats
    QPen pen;
    const QColor penBaseColor(176, 176, 176);
    if ((mBeat % 4) == 0)
    {
        pen.setColor(penBaseColor.darker(125));
        pen.setWidthF(TIMELINE_MEASURE_LINE_WIDTH);
    }
    else
    {
        pen.setColor(penBaseColor);
        pen.setWidth(0);
    }
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw the vertical line
    painter->drawLine(0.0f,
                      0.0f,
                      0.0f,
                      static_cast<float>(mNumLanes) * TIMELINE_LANE_HEIGHT);

    // Draw the beat number for each measure
    if ((mBeat % 4) == 0)
    {
        QFont font = painter->font();
        font.setPixelSize(TIMELINE_BEAT_NUMBER_FONT_HEIGHT);
        font.setBold(false);
        painter->setFont(font);
        painter->setPen(penBaseColor.darker(200));
        painter->drawText(-TIMELINE_BEAT_NUMBER_BLOCK_WIDTH * 0.5f,
                          -TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT,
                          TIMELINE_BEAT_NUMBER_BLOCK_WIDTH,
                          TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT,
                          Qt::AlignHCenter | Qt::AlignVCenter,
                          QString("%1").arg(mBeat));
    }
}

//----------------------------------------------------------------------------------------

void TimelineBackgroundBeatLineGraphicsItem::SetPositionFromBeat()
{
    float position = static_cast<float>(mBeat) * (mHorizontalScale * TIMELINE_BEAT_WIDTH);
    setPos(position, 0.0f);
}
