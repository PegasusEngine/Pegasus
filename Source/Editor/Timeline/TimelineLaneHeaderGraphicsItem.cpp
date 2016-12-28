/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineLaneHeaderGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	29th September 2013
//! \brief	Graphics item representing the header block on the left of a lane in the timeline

#include "Timeline/TimelineLaneHeaderGraphicsItem.h"
#include "Timeline/TimelineSizes.h"
#include <QAbstractScrollArea>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>


#define TIMELINE_LANE_HEADER_ITEM_Z_VALUE 3.0f

//! Table of base colors for the lane headers when they are created
static const QColor LANE_HEADER_BASE_COLORS[] = 
    {
        QColor(255, 0  , 0  ),
        QColor(0  , 255, 0  ),
        QColor(255, 255, 0  ),
        QColor(0  , 0  , 255),
        QColor(255, 0  , 255),
        QColor(0  , 255, 255),
        QColor(255, 128, 0  ),
        QColor(0  , 255, 128),
        QColor(128, 0  , 255),
        QColor(255, 0  , 128),
        QColor(0  , 128, 255),
        QColor(128, 255, 0  )
    };

//! Number of base colors for the lane headers when they are created
static const unsigned int NUM_LANE_HEADER_BASE_COLORS = sizeof(LANE_HEADER_BASE_COLORS) / sizeof(QColor);

//----------------------------------------------------------------------------------------

TimelineLaneHeaderGraphicsItem::TimelineLaneHeaderGraphicsItem(unsigned int lane)
:   QGraphicsItem(),
    mLane(lane)
{
    // Set the initial position
    SetPositionFromLane();

    // Set the initial name of the lane
    SetDefaultName(false);

    // Set the initial color from the lane number
    const unsigned int colorIndex = lane % NUM_LANE_HEADER_BASE_COLORS;
    QColor baseColor = LANE_HEADER_BASE_COLORS[colorIndex];
    ComputeColorsFromBase(baseColor);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the block to be the most in the background
    setZValue(TIMELINE_LANE_HEADER_ITEM_Z_VALUE);
}

//----------------------------------------------------------------------------------------

TimelineLaneHeaderGraphicsItem::~TimelineLaneHeaderGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::SetLane(unsigned int lane)
{
    mLane = lane;

    // Update the position
    SetPositionFromLane();

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::SetName(const QString & name)
{
    //! \todo Check for invalid or redundant name

    mName = name;

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::SetColor(const QColor & color)
{
    // Update the colors
    ComputeColorsFromBase(color);

    // Invalidate the cache of the graphics item.
    // If not done manually here, setCacheMode(NoCache) would be necessary
    // in the constructor, resulting in poor performances
    update(boundingRect());
}

//----------------------------------------------------------------------------------------

QRectF TimelineLaneHeaderGraphicsItem::boundingRect() const
{
    return QRectF(-TIMELINE_LANE_HEADER_WIDTH,
                  0.0f,
                  TIMELINE_LANE_HEADER_WIDTH,
                  TIMELINE_LANE_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
    Q_UNUSED(widget);

    // Draw the background gradient of the header
    QLinearGradient gradient(0.0f,
                             0.0f,
                             0.0f,
                             TIMELINE_LANE_HEIGHT);
    gradient.setColorAt(0, mTopColor);
    gradient.setColorAt(1, mBottomColor);
    painter->fillRect(-TIMELINE_LANE_HEADER_WIDTH,
                      0.0f,
                      TIMELINE_LANE_HEADER_WIDTH,
                      TIMELINE_LANE_HEIGHT,
                      gradient);

    // Draw the label of the header
    QFont font = painter->font();
    font.setPixelSize(TIMELINE_LANE_HEADER_FONT_HEIGHT);
    font.setBold(false);
    painter->setFont(font);
    painter->setPen(Qt::black);
    const float fontHeightScale = 1.25f;       // To handle text below the base line
    const float textMargin = (TIMELINE_BLOCK_HEIGHT - fontHeightScale * static_cast<float>(TIMELINE_LANE_HEADER_FONT_HEIGHT)) * 0.5f;
    QRectF textRect(-TIMELINE_LANE_HEADER_WIDTH + textMargin * 2,       // Added extra space on the left
                    textMargin,
                    TIMELINE_LANE_HEADER_WIDTH - 3.0f * textMargin,     // Takes the extra space on the left into account
                    TIMELINE_BLOCK_HEIGHT - 2.0f * textMargin);
    painter->drawText(textRect, mName);
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::SetPositionFromLane()
{
    setPos(0.0f, mLane * TIMELINE_LANE_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::ComputeColorsFromBase(const QColor & color)
{
    mTopColor = color.lighter(170);
    mBottomColor = mTopColor.darker(120);
}

//----------------------------------------------------------------------------------------

void TimelineLaneHeaderGraphicsItem::SetDefaultName(bool updateView)
{
    mName = QString("Lane %1").arg(mLane);

    if (updateView)
    {
        // Invalidate the cache of the graphics item.
        // If not done manually here, setCacheMode(NoCache) would be necessary
        // in the constructor, resulting in poor performances
        update(boundingRect());
    }
}
