/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#include "Timeline/TimelineBlockGraphicsItem.h"
#include "Timeline/TimelineUndoCommands.h"
#include "Timeline/TimelineSizes.h"

#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


//! Depth of the block when drawn. Positive since it needs to be rendered in front of the grid at least
#define TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE                10.0f

unsigned int TimelineBlockGraphicsItem::sCurrentBlockID = 0;
unsigned int TimelineBlockGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::TimelineBlockGraphicsItem(Pegasus::Timeline::IBlockProxy * blockProxy,
                                                     unsigned int lane,
                                                     float horizontalScale,
                                                     QUndoStack* undoStack)
:   QGraphicsObject(),
    mBlockProxy(blockProxy),
    mName((blockProxy != nullptr) ? blockProxy->GetEditorString() : ""),
    mEnableUndo(true),
    mUndoStack(undoStack)
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

    // Set the initial position and duration, and update the scaled position and length
    SetDuration(blockProxy->GetDuration(), false);
    SetLane(lane, false);
    SetBeat(blockProxy->GetBeat(), true);

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

    //! Assign a unique ID to the block for merging undo commands when moving the block
    mBlockID = sCurrentBlockID++;
}

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::~TimelineBlockGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLane(unsigned int lane, bool updateItem)
{
    mEnableUndo = false;

    mLane = lane;

    // Update the vertical position
    SetYFromLane();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mX, mY);
        update(boundingRect());

        emit BlockMoved();
    }

    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBeat(Pegasus::Timeline::Beat beat, bool updateItem)
{
    mEnableUndo = false;

    mBeat = beat;

    // Update the scaled position
    SetXFromBeat();

    // Update the graphics item, so it is redrawn at the right location
    if (updateItem)
    {
        // Set the new position of the graphics item
        setPos(mX, mY);
        update(boundingRect());

        emit BlockMoved();
    }

    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetDuration(Pegasus::Timeline::Duration duration, bool updateItem)
{
    if (duration == 0)
    {
        ED_FAILSTR("Invalid duration (%d) for the timeline graphics item. It should be > 0.", duration);
        mDuration = 1;
    }
    else
    {
        mDuration = duration;
    }

    // Update the scaled length
    SetLengthFromDuration();

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
    SetXFromBeat();
    SetLengthFromDuration();

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
                  mLength,
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
                      mLength,
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
                    mLength - 3.0f * textMargin,        // Takes the extra space on the left into account
                    TIMELINE_BLOCK_HEIGHT - 2.0f * textMargin);
    painter->drawText(textRect, mName);
}

//----------------------------------------------------------------------------------------

QVariant TimelineBlockGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        // Called when the position of the block changes by even one pixel.
        // This section returns a new position if it needs override (block staying on lane,
        // snapping), otherwise returns the current value.
        // In all cases, Pegasus is informed of the update
        case ItemPositionChange:
            {
                if (mEnableUndo)
                {
                    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
                    if (laneProxy == nullptr)
                    {
                        ED_FAILSTR("Unable to move the block \"%s\" since it has no associated lane", mBlockProxy->GetEditorString());
                        break;
                    }
                    Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
                    if (timelineProxy == nullptr)
                    {
                        ED_FAILSTR("Unable to move the block \"%s\" since it has no associated timeline", mBlockProxy->GetEditorString());
                        break;
                    }

                    // Compute the desired beat and lane from the mouse position
                    QPointF newMousePos = value.toPointF();
                    Pegasus::Timeline::Beat newBeat = GetBeatFromX(newMousePos.x());
                    unsigned int newLane = GetLaneFromY(newMousePos.y());

                    // Apply snapping
                    const unsigned int snapNumTicks = Editor::GetInstance().GetTimelineDockWidget()->GetSnapNumTicks();
                    newBeat = (newBeat / snapNumTicks) * snapNumTicks;

                    // If the beat and lane have not changed (movement smaller than a tick),
                    // keep the old location and tell the item we overrode the position
                    if ((newBeat == mBeat) && (newLane == mLane))
                    {
                        return QPointF(mX, mY);
                    }

                    // Determine if the block is part of a multiple selection
                    bool isMultipleSelection = false;
                    QGraphicsScene * const parentScene = scene();
                    QList<QGraphicsItem *> selectedItems;
                    if (parentScene != nullptr)
                    {
                        selectedItems = parentScene->selectedItems();
                        if (selectedItems.count() >= 2)
                        {
                            ED_ASSERTSTR(selectedItems.contains(this), "Invalid block selection");
                            isMultipleSelection = true;
                        }
                    }

                    // Test if the block fits in its new location
                    Pegasus::Timeline::ILaneProxy * newLaneProxy = timelineProxy->GetLane(newLane);
                    if (newLaneProxy == nullptr)
                    {
                        newLaneProxy = laneProxy;
                    }
                    if (newLaneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
                    {
                        // If the block fits
                    
                        // Create the undo command
                        QUndoCommand * undoCommand = nullptr;
                        if (isMultipleSelection)
                        {
                            undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
                                                                                       (newLane != mLane), newLane, newBeat,
                                                                                       sMouseClickID);
                        }
                        else
                        {
                            undoCommand = new TimelineSetBlockPositionUndoCommand(this,
                                                                                  (newLane != mLane), newLane, newBeat,
                                                                                  sMouseClickID);
                        }
        
                        mUndoStack->push(undoCommand);

                        // If the new coordinates differ from the new mouse position,
                        // tell the item we overrode the position
                        if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
                        {
                            return QPointF(mX, mY);
                        }
                    }
                    else
                    {
                        // If the block does not fit

                        if ((newLane != mLane) && (newBeat != mBeat))
                        {
                            // If the lane and the beats have changed, test if the block fits in the original lane.
                            // In that case, just affect the beat
                            if (laneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
                            {
                                // Create the undo command
                                QUndoCommand * undoCommand = nullptr;
                                if (isMultipleSelection)
                                {
                                    undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
                                                                                               false, mLane, newBeat,
                                                                                               sMouseClickID);
                                }
                                else
                                {
                                    undoCommand = new TimelineSetBlockPositionUndoCommand(this,
                                                                                          false, mLane, newBeat,
                                                                                          sMouseClickID);
                                }
        
                                mUndoStack->push(undoCommand);

                                // If the new coordinates differ from the new mouse position,
                                // tell the item we overrode the position
                                if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
                                {
                                    return QPointF(mX, mY);
                                }
                            }
                            else
                            {
                                // If the block does not fit in the new location in a different lane,
                                // keep the old location and tell the item we overrode the position
                                return QPointF(mX, mY);
                            }
                        }
                        else
                        {
                            // If the block does not fit in the new location in the same lane,
                            // keep the old location and tell the item we overrode the position
                            return QPointF(mX, mY);
                        }
                    }
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
    // Consider the next time we move the same block as a new undo command
    // (do not test for left button, it does not seem to work)
    sMouseClickID++;

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
    emit(DoubleClicked(mBlockProxy));
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetXFromBeat()
{
    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
    if (laneProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
        if (timelineProxy != nullptr)
        {
            mX = (mBeat * timelineProxy->GetRcpNumTicksPerBeat()) * mHorizontalScale * TIMELINE_BEAT_WIDTH;
        }
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Timeline::Beat TimelineBlockGraphicsItem::GetBeatFromX(float x) const
{
    int beat = static_cast<int>(mBeat);

    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
    if (laneProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
        if (timelineProxy != nullptr)
        {
            beat = static_cast<int>(floor((x / (mHorizontalScale * TIMELINE_BEAT_WIDTH)) * timelineProxy->GetNumTicksPerBeatFloat()));
            if (beat < 0)
            {
                beat = 0;
            }
        }
    }

    return static_cast<Pegasus::Timeline::Beat>(beat);
}

//----------------------------------------------------------------------------------------
    
void TimelineBlockGraphicsItem::SetYFromLane()
{
    mY = TIMELINE_BLOCK_MARGIN_HEIGHT + mLane * TIMELINE_LANE_HEIGHT;
}

//----------------------------------------------------------------------------------------

unsigned int TimelineBlockGraphicsItem::GetLaneFromY(float lanePosition) const
{
    int lane = static_cast<int>(floor((lanePosition - TIMELINE_BLOCK_MARGIN_HEIGHT) / TIMELINE_LANE_HEIGHT));
    if (lane < 0)
    {
        lane = 0;
    }

    // Clamp to avoid blocks to move to an invalid lane
    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
    if (laneProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
        if (timelineProxy != nullptr)
        {
            const int numLanes = static_cast<int>(timelineProxy->GetNumLanes());
            if (lane >= numLanes)
            {
                lane = numLanes - 1;
            }
        }
    }

    return static_cast<unsigned int>(lane);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLengthFromDuration()
{
    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
    if (laneProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
        if (timelineProxy != nullptr)
        {
            mLength = (mDuration * timelineProxy->GetRcpNumTicksPerBeat()) * (mHorizontalScale * TIMELINE_BEAT_WIDTH);
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBaseLengthFromPixelLength()
{
    Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
    if (laneProxy != nullptr)
    {
        Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
        if (timelineProxy != nullptr)
        {
            mDuration = (mLength / (mHorizontalScale * TIMELINE_BEAT_WIDTH)) * timelineProxy->GetNumTicksPerBeatFloat();
        }
    }
}
