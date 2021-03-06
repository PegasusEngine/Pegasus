/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineCursorGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	27th September 2013
//! \brief	Graphics item representing the currently selected time as a vertical bar on top of the timeline


#include "Timeline/TimelineCursorGraphicsItem.h"
#include "Timeline/TimelineSizes.h"
#include <QPainter>


#define TIMELINE_CURSOR_Z_GRAPHICS_VALUE  2.9f

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
        ED_FAILSTR("Invalid horizontal scale (%f) for the timeline cursor graphics item. It should be > 0.0f.", horizontalScale);
        mHorizontalScale = 1.0f;
    }
    else
    {
        mHorizontalScale = horizontalScale;
    }

    // Set the initial scaled position
    SetPositionFromBeat();

    // No caching, as the cursor is a very simple primitive
    setCacheMode(NoCache);

    setZValue(TIMELINE_CURSOR_Z_GRAPHICS_VALUE);
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
        ED_FAILSTR("Invalid position of the timeline cursor graphics item (%f). It should be >= 0.0f.", beat);
        mBeat = 0.0f;
    }
    else
    {
        mBeat = beat;
    }

    //! \todo Test for beats farther than the end of the timeline

    //! Update the position
    SetPositionFromBeat();
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

    // No cache management
}

//----------------------------------------------------------------------------------------

void TimelineCursorGraphicsItem::SetHorizontalScale(float scale)
{
    // No assertion for invalid scale (performance issue).
    // The caller performs the assertion test.
    mHorizontalScale = scale;

    //! Update the scaled position
    SetPositionFromBeat();

    // No cache management
}

//----------------------------------------------------------------------------------------

QRectF TimelineCursorGraphicsItem::boundingRect() const
{
    // Multiplier for the width of the cursor, so no rendering bugs occurs when moving the cursor
    const float widthScale = 20.0f;

    return QRectF(-TIMELINE_CURSOR_LINE_WIDTH * 0.5f * widthScale,
                  0.0f,
                  TIMELINE_CURSOR_LINE_WIDTH * 0.5f * widthScale,
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
