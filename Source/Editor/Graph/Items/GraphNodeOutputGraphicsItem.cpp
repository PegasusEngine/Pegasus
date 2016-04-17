/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeOuputGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	17th April 2016
//! \brief	Graphics item representing one node output in the graph

#include "Graph/Items/GraphNodeOutputGraphicsItem.h"
#include "Graph/Items/GraphGraphicsItemDefs.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


//unsigned int GraphNodeOutputGraphicsItem::sCurrentNodeID = 0;
//unsigned int GraphNodeOutputGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

GraphNodeOutputGraphicsItem::GraphNodeOutputGraphicsItem(QGraphicsScene* scene, QUndoStack* undoStack)
:   QGraphicsObject(),
    mUndoStack(undoStack)
{
    // Make the block movable and selectable with the mouse
    //setFlag(ItemIsMovable);
    //setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the item movements
    //setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the block (positive since it needs to be rendered
    // in front of the grid at least)
    //******setZValue(TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE);
}

//----------------------------------------------------------------------------------------

GraphNodeOutputGraphicsItem::~GraphNodeOutputGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

QRectF GraphNodeOutputGraphicsItem::boundingRect() const
{
    return QRectF(-GRAPHITEM_OUTPUT_HALF_WIDTHF, -GRAPHITEM_OUTPUT_HALF_WIDTHF,
                  GRAPHITEM_OUTPUT_WIDTHF, GRAPHITEM_OUTPUT_WIDTHF);
}

//----------------------------------------------------------------------------------------

void GraphNodeOutputGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::NoBrush);

    static const QPointF sPixOrigin(-GRAPHITEM_OUTPUT_HALF_WIDTHF, -GRAPHITEM_OUTPUT_HALF_WIDTHF);

    QPixmap nodeOutputPix(":/GraphEditor/NodeOutput16.png");
    painter->drawPixmap(sPixOrigin, /****/nodeOutputPix);
}

//----------------------------------------------------------------------------------------

//QVariant GraphNodeOutputGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//    switch (change)
//    {
//        // Called when the position of the node changes by even one pixel.
//        // This section returns a new position if it needs override (collision detection),
//        // otherwise returns the current value.
//        // In all cases, Pegasus is informed of the update
//        case ItemPositionChange:
//            {
//                /*****/
//                QPointF newPos = value.toPointF();
//                QPointF newGridPos(floorf((newPos.x() + 32.0f) / 64.0f) * 64.0f - 32.0f,
//                                   floorf((newPos.y() + 32.0f) / 64.0f) * 64.0f - 32.0f);
//                return newGridPos;
//                /*****/
//
//                if (mEnableUndo)
//                {
//                    /*Pegasus::Timeline::ILaneProxy * laneProxy = mBlockProxy->GetLane();
//                    if (laneProxy == nullptr)
//                    {
//                        ED_FAILSTR("Unable to move the block \"%s\" since it has no associated lane", mBlockProxy->GetEditorString());
//                        break;
//                    }
//                    Pegasus::Timeline::ITimelineProxy * timelineProxy = laneProxy->GetTimeline();
//                    if (timelineProxy == nullptr)
//                    {
//                        ED_FAILSTR("Unable to move the block \"%s\" since it has no associated timeline", mBlockProxy->GetEditorString());
//                        break;
//                    }*/
//
//                    //// Compute the desired beat and lane from the mouse position
//                    //QPointF newMousePos = value.toPointF();
//                    //Pegasus::Timeline::Beat newBeat = GetBeatFromX(newMousePos.x());
//                    //unsigned int newLane = GetLaneFromY(newMousePos.y());
//
//                    //// Apply snapping
//                    //const unsigned int snapNumTicks = Editor::GetInstance().GetTimelineDockWidget()->GetSnapNumTicks();
//                    //newBeat = (newBeat / snapNumTicks) * snapNumTicks;
//
//                    //// If the beat and lane have not changed (movement smaller than a tick),
//                    //// keep the old location and tell the item we overrode the position
//                    //if ((newBeat == mBeat) && (newLane == mLane))
//                    //{
//                    //    return QPointF(mX, mY);
//                    //}
//
//                    //// Determine if the block is part of a multiple selection
//                    //bool isMultipleSelection = false;
//                    //QGraphicsScene * const parentScene = scene();
//                    //QList<QGraphicsItem *> selectedItems;
//                    //if (parentScene != nullptr)
//                    //{
//                    //    selectedItems = parentScene->selectedItems();
//                    //    if (selectedItems.count() >= 2)
//                    //    {
//                    //        ED_ASSERTSTR(selectedItems.contains(this), "Invalid block selection");
//                    //        isMultipleSelection = true;
//                    //    }
//                    //}
//
//                    //// Test if the block fits in its new location
//                    //Pegasus::Timeline::ILaneProxy * newLaneProxy = timelineProxy->GetLane(newLane);
//                    //if (newLaneProxy == nullptr)
//                    //{
//                    //    newLaneProxy = laneProxy;
//                    //}
//                    //if (newLaneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
//                    //{
//                    //    // If the block fits
//                    //
//                    //    // Create the undo command
//                    //    QUndoCommand * undoCommand = nullptr;
//                    //    if (isMultipleSelection)
//                    //    {
//                    //        undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
//                    //                                                                   (newLane != mLane), newLane, newBeat,
//                    //                                                                   sMouseClickID);
//                    //    }
//                    //    else
//                    //    {
//                    //        undoCommand = new TimelineSetBlockPositionUndoCommand(this,
//                    //                                                              (newLane != mLane), newLane, newBeat,
//                    //                                                              sMouseClickID);
//                    //    }
//        
//                    //    mUndoStack->push(undoCommand);
//
//                    //    // If the new coordinates differ from the new mouse position,
//                    //    // tell the item we overrode the position
//                    //    if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
//                    //    {
//                    //        return QPointF(mX, mY);
//                    //    }
//                    //}
//                    //else
//                    //{
//                    //    // If the block does not fit
//
//                    //    if ((newLane != mLane) && (newBeat != mBeat))
//                    //    {
//                    //        // If the lane and the beats have changed, test if the block fits in the original lane.
//                    //        // In that case, just affect the beat
//                    //        if (laneProxy->IsBlockFitting(mBlockProxy, newBeat, mBlockProxy->GetDuration()))
//                    //        {
//                    //            // Create the undo command
//                    //            QUndoCommand * undoCommand = nullptr;
//                    //            if (isMultipleSelection)
//                    //            {
//                    //                undoCommand = new TimelineSetMultiBlockPositionUndoCommand(this, selectedItems,
//                    //                                                                           false, mLane, newBeat,
//                    //                                                                           sMouseClickID);
//                    //            }
//                    //            else
//                    //            {
//                    //                undoCommand = new TimelineSetBlockPositionUndoCommand(this,
//                    //                                                                      false, mLane, newBeat,
//                    //                                                                      sMouseClickID);
//                    //            }
//        
//                    //            mUndoStack->push(undoCommand);
//
//                    //            // If the new coordinates differ from the new mouse position,
//                    //            // tell the item we overrode the position
//                    //            if ( (mX != newMousePos.x()) || (mY != newMousePos.y()) )
//                    //            {
//                    //                return QPointF(mX, mY);
//                    //            }
//                    //        }
//                    //        else
//                    //        {
//                    //            // If the block does not fit in the new location in a different lane,
//                    //            // keep the old location and tell the item we overrode the position
//                    //            return QPointF(mX, mY);
//                    //        }
//                    //    }
//                    //    else
//                    //    {
//                    //        // If the block does not fit in the new location in the same lane,
//                    //        // keep the old location and tell the item we overrode the position
//                    //        return QPointF(mX, mY);
//                    //    }
//                    //}
//                }
//            }
//            break;
//
//        default:
//            break;
//    };
//
//    return QGraphicsItem::itemChange(change, value);
//}

//----------------------------------------------------------------------------------------

void GraphNodeOutputGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------

void GraphNodeOutputGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Consider the next time we move the same node as a new undo command
    // (do not test for left button, it does not seem to work)
    //sMouseClickID++;

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

//----------------------------------------------------------------------------------------

void GraphNodeOutputGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
    //*****emit(DoubleClicked(mBlockProxy));
}

