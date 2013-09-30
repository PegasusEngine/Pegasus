/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineCursorGraphicsItem.cpp
//! \author	Kevin Boulanger
//! \date	27th September 2013
//! \brief	Graphics item representing the currently selected time as a vertical bar on top of the timeline


#include "Timeline/TimelineCursorGraphicsItem.h"
#include "Timeline/TimelineSizes.h"
#include <QPainter>


TimelineCursorGraphicsItem::TimelineCursorGraphicsItem(unsigned int numLanes, float horizontalScale)
:   QGraphicsItem(),
    mBeat(0.0f)
{
    if (numLanes == 0)
    {
        ED_FAILSTR("Invalid number of lanes (0) for the timeline cursor graphics item. It should be > 0.");
        mNumLanes = 1;
    }
    else
    {
        mNumLanes = numLanes;
    }

    if (horizontalScale <= 0.0f)
    {
        ED_FAILSTRF("Invalid horizontal scale (%f) for the timeline cursor graphics item. It should be > 0.0f.", horizontalScale);
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
    setZValue(20.0f);
}

//----------------------------------------------------------------------------------------

TimelineCursorGraphicsItem::~TimelineCursorGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void TimelineCursorGraphicsItem::SetBeat(float beat)
{
    if (beat < 0.0f)
    {
        ED_FAILSTRF("Invalid position of the timeline cursor graphics item (%f). It should be >= 0.0f.", beat);
        mBeat = 0.0f;
    }
    else
    {
        mBeat = beat;
    }

    //! \todo Test for beats farther than the end of the timeline

    //! Update the position
    SetPositionFromBeat();

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

void TimelineCursorGraphicsItem::SetNumLanes(unsigned int numLanes)
{
    if (numLanes == 0)
    {
        ED_FAILSTR("Invalid number of lanes (0) for the timeline cursor graphics item. It should be > 0.");
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

void TimelineCursorGraphicsItem::SetHorizontalScale(float scale)
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

QRectF TimelineCursorGraphicsItem::boundingRect() const
{
    return QRectF(-TIMELINE_CURSOR_LINE_WIDTH * 0.5f,
                  0.0f,
                  TIMELINE_CURSOR_LINE_WIDTH * 0.5f,
                  mNumLanes * TIMELINE_LANE_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineCursorGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    // Set the pen and brush parameters
    QPen pen;
    pen.setColor(QColor(255, 0, 0));
    pen.setWidthF(TIMELINE_CURSOR_LINE_WIDTH);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw the vertical line
    painter->drawLine(0.0f,
                      0.0f,
                      0.0f,
                      static_cast<float>(mNumLanes) * TIMELINE_LANE_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineCursorGraphicsItem::SetPositionFromBeat()
{
    float position = mBeat * (mHorizontalScale * TIMELINE_BEAT_WIDTH);
    setPos(position, 0.0f);
}
