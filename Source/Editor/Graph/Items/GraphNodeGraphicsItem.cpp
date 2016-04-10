/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	02nd April 2016
//! \brief	Graphics item representing one node in the graph

#include "Graph/Items/GraphNodeGraphicsItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


unsigned int GraphNodeGraphicsItem::sCurrentNodeID = 0;
unsigned int GraphNodeGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem::GraphNodeGraphicsItem(QUndoStack* undoStack)
:   QGraphicsObject(),
    mUndoStack(undoStack)
{
    // Make the block movable and selectable with the mouse
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the item movements
    //setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the block (positive since it needs to be rendered
    // in front of the grid at least)
    //******setZValue(TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE);

    //! Assign a unique ID to the block for merging undo commands when moving the block
    mNodeID = sCurrentNodeID++;

}

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem::~GraphNodeGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

QRectF GraphNodeGraphicsItem::boundingRect() const
{
    return QRectF(0.0f,
                  0.0f,
                  /*****mLength*/50.0f,
                  /*****TIMELINE_BLOCK_HEIGHT*/100.0f);
}

//----------------------------------------------------------------------------------------

void GraphNodeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    //const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    // Set the outline of the block to a solid line with width 1 for any zoom (cosmetic pen) and square corners.
    // If the LOD is too low, remove the outline, otherwise the rendering is messy and aliased
    //if (lod > 0.5f)
    //{
    //    QColor outlineColor = mBaseColor.darker(200);
    //    QPen outlinePen(QBrush(outlineColor), 0, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    //    painter->setPen(outlinePen);
    //}
    //else
    //{
    //    painter->setPen(Qt::NoPen);
    //}

    // Set the fill color, darker when selected, lighter when hovered by the mouse
    //QColor fillColor = (option->state & QStyle::State_Selected) ? mBaseColor.darker(150) : mBaseColor;
    QColor fillColor = Qt::blue;
    if (option->state & QStyle::State_MouseOver)
    {
        fillColor = fillColor.lighter(125);
    }
    painter->setBrush(fillColor);

    // Draw the block
    painter->drawRect(0.0f,
                      0.0f,
                      /**mLength*/50.0f,
                      /**TIMELINE_BLOCK_HEIGHT*/100.0f);

    //// Draw the label of the block
    //QFont font = painter->font();
    //font.setPixelSize(TIMELINE_BLOCK_FONT_HEIGHT);
    //font.setBold(false);
    //painter->setFont(font);
    //painter->setPen(Qt::black);
    //const float fontHeightScale = 1.25f;                // To handle text below the base line
    //const float textMargin = (TIMELINE_BLOCK_HEIGHT - fontHeightScale * (float)TIMELINE_BLOCK_FONT_HEIGHT) * 0.5f;
    //QRectF textRect(textMargin * 2,                     // Added extra space on the left
    //                textMargin,
    //                mLength - 3.0f * textMargin,        // Takes the extra space on the left into account
    //                TIMELINE_BLOCK_HEIGHT - 2.0f * textMargin);
    //painter->drawText(textRect, mName);
}

//----------------------------------------------------------------------------------------

QVariant GraphNodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        // Called when the position of the node changes by even one pixel.
        // This section returns a new position if it needs override (collision detection),
        // otherwise returns the current value.
        // In all cases, Pegasus is informed of the update
        case ItemPositionChange:
            {
                if (mEnableUndo)
                {
                    /*Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
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
                    }*/

                    //// Compute the desired beat and lane from the mouse position
                    //QPointF newMousePos = value.toPointF();
                    //Pegasus::Timeline::Beat newBeat = GetBeatFromX(newMousePos.x());
                    //unsigned int newLane = GetLaneFromY(newMousePos.y());

                    //// Apply snapping
                    //const unsigned int snapNumTicks = Editor::GetInstance().GetTimelineDockWidget()->GetSnapNumTicks();
                    //newBeat = (newBeat / snapNumTicks) * snapNumTicks;

                    //// If the beat and lane have not changed (movement smaller than a tick),
                    //// keep the old location and tell the item we overrode the position
                    //if ((newBeat == mBeat) && (newLane == mLane))
                    //{
                    //    return QPointF(mX, mY);
                    //}

                    //// Determine if the block is part of a multiple selection
                    //bool isMultipleSelection = false;
                    //QGraphicsScene * const parentScene = scene();
                    //QList<QGraphicsItem *> selectedItems;
                    //if (parentScene != nullptr)
                    //{
                    //    selectedItems = parentScene->selectedItems();
                    //    if (selectedItems.count() >= 2)
                    //    {
                    //        ED_ASSERTSTR(selectedItems.contains(this), "Invalid block selection");
                    //        isMultipleSelection = true;
                    //    }
                    //}

                    //// Test if the block fits in its new location
                    //Pegasus::Timeline::ILaneProxy * newLaneProxy = timelineProxy->GetLane(newLane);
                    //if (newLaneProxy == nullptr)
                    //{
                    //    newLaneProxy = laneProxy;
                    //}
                    //if (newLaneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
                    //{
                    //    // If the block fits
                    //
                    //    // Create the undo command
                    //    QUndoCommand * undoCommand = nullptr;
                    //    if (isMultipleSelection)
                    //    {
                    //        undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
                    //                                                                   (newLane != mLane), newLane, newBeat,
                    //                                                                   sMouseClickID);
                    //    }
                    //    else
                    //    {
                    //        undoCommand = new TimelineSetBlockPositionUndoCommand(this,
                    //                                                              (newLane != mLane), newLane, newBeat,
                    //                                                              sMouseClickID);
                    //    }
        
                    //    mUndoStack->push(undoCommand);

                    //    // If the new coordinates differ from the new mouse position,
                    //    // tell the item we overrode the position
                    //    if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
                    //    {
                    //        return QPointF(mX, mY);
                    //    }
                    //}
                    //else
                    //{
                    //    // If the block does not fit

                    //    if ((newLane != mLane) && (newBeat != mBeat))
                    //    {
                    //        // If the lane and the beats have changed, test if the block fits in the original lane.
                    //        // In that case, just affect the beat
                    //        if (laneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
                    //        {
                    //            // Create the undo command
                    //            QUndoCommand * undoCommand = nullptr;
                    //            if (isMultipleSelection)
                    //            {
                    //                undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
                    //                                                                           false, mLane, newBeat,
                    //                                                                           sMouseClickID);
                    //            }
                    //            else
                    //            {
                    //                undoCommand = new TimelineSetBlockPositionUndoCommand(this,
                    //                                                                      false, mLane, newBeat,
                    //                                                                      sMouseClickID);
                    //            }
        
                    //            mUndoStack->push(undoCommand);

                    //            // If the new coordinates differ from the new mouse position,
                    //            // tell the item we overrode the position
                    //            if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
                    //            {
                    //                return QPointF(mX, mY);
                    //            }
                    //        }
                    //        else
                    //        {
                    //            // If the block does not fit in the new location in a different lane,
                    //            // keep the old location and tell the item we overrode the position
                    //            return QPointF(mX, mY);
                    //        }
                    //    }
                    //    else
                    //    {
                    //        // If the block does not fit in the new location in the same lane,
                    //        // keep the old location and tell the item we overrode the position
                    //        return QPointF(mX, mY);
                    //    }
                    //}
                }
            }
            break;

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}

//----------------------------------------------------------------------------------------

void GraphNodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------

void GraphNodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Consider the next time we move the same node as a new undo command
    // (do not test for left button, it does not seem to work)
    sMouseClickID++;

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------

void GraphNodeGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
    //*****emit(DoubleClicked(mBlockProxy));
}

