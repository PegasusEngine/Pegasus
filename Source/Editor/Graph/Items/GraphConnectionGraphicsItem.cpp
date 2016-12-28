/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphConnectionGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	03rd May 2016
//! \brief	Graphics item representing a connection between two nodes in the graph

#include "Graph/Items/GraphConnectionGraphicsItem.h"
#include "Graph/Items/GraphGraphicsItemDefs.h"
#include "Graph/Items/GraphNodeInputGraphicsItem.h"
#include "Graph/Items/GraphNodeOutputGraphicsItem.h"

#include <QAbstractScrollArea>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


//unsigned int GraphNodeGraphicsItem::sCurrentNodeID = 0;
//unsigned int GraphNodeGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

GraphConnectionGraphicsItem::GraphConnectionGraphicsItem(GraphNodeOutputGraphicsItem* srcOutput,
                                                         GraphNodeInputGraphicsItem* dstInput,
                                                         QGraphicsScene* scene,
                                                         QUndoStack* undoStack)
:   QGraphicsObject()
,   mUndoStack(undoStack)
,   mSrcOutput(srcOutput)
,   mDstInput(dstInput)
{
    ED_ASSERTSTR(srcOutput != nullptr, "Trying to create a connection between two nodes, but the source node output is null");
    ED_ASSERTSTR(dstInput != nullptr, "Trying to create a connection between two nodes, but the destination node input is null");

    // Make the block movable and selectable with the mouse
    //setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the item movements
    //setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the connection (behind inputs and outputs)
    setZValue(GRAPHITEM_CONNECTION_Z_VALUE);

    //! Assign a unique ID to the block for merging undo commands when moving the block
    //mNodeID = sCurrentNodeID++;

    // Update the output and input to know we are connected to them
    srcOutput->AddConnection(this);
    dstInput->SetConnection(this);
}

//----------------------------------------------------------------------------------------

GraphConnectionGraphicsItem::~GraphConnectionGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void GraphConnectionGraphicsItem::AdjustGeometry()
{
    // Tell the paint system that the geometry is change, updating the bounding box,
    // and triggering a rendering
    prepareGeometryChange();
}

//----------------------------------------------------------------------------------------

QRectF GraphConnectionGraphicsItem::boundingRect() const
{
    const QPointF srcPos = mSrcOutput->scenePos();
    const QPointF dstPos = mDstInput->scenePos();

    const qreal minX = srcPos.x() < dstPos.x() ? srcPos.x() : dstPos.x();
    const qreal maxX = srcPos.x() > dstPos.x() ? srcPos.x() : dstPos.x();
    const qreal minY = srcPos.y() < dstPos.y() ? srcPos.y() : dstPos.y();
    const qreal maxY = srcPos.y() > dstPos.y() ? srcPos.y() : dstPos.y();

    return QRectF(minX
                  - GRAPHITEM_CONNECTION_CONTROL_VECTOR_LENGTH
                  - GRAPHITEM_CONNECTION_MARGIN,
                  minY
                  - GRAPHITEM_CONNECTION_MARGIN,
                  GRAPHITEM_CONNECTION_MARGIN
                  + GRAPHITEM_CONNECTION_CONTROL_VECTOR_LENGTH
                  + (maxX - minX)
                  + GRAPHITEM_CONNECTION_CONTROL_VECTOR_LENGTH
                  + GRAPHITEM_CONNECTION_MARGIN,
                  GRAPHITEM_CONNECTION_MARGIN
                  + (maxY - minY)
                  + GRAPHITEM_CONNECTION_MARGIN);
}

//----------------------------------------------------------------------------------------

void GraphConnectionGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    const QPointF srcPos = mSrcOutput->scenePos();
    const QPointF dstPos = mDstInput->scenePos();

    QPen pathPen;
    pathPen.setWidth(GRAPHITEM_CONNECTION_WIDTH);
    pathPen.setColor(GRAPHITEM_CONNECTION_COLOR);
    painter->setPen(pathPen);

    QPainterPath path;
    path.moveTo(srcPos);
    path.cubicTo(srcPos.x() + GRAPHITEM_CONNECTION_CONTROL_VECTOR_LENGTH, srcPos.y(),
                 dstPos.x() - GRAPHITEM_CONNECTION_CONTROL_VECTOR_LENGTH, dstPos.y(),
                 dstPos.x(), dstPos.y());
    painter->drawPath(path);
}

//----------------------------------------------------------------------------------------

//QVariant GraphConnectionGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
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
//                //QPointF newPos = value.toPointF();
//                //QPointF newGridPos(floorf((newPos.x() + 32.0f) / 64.0f) * 64.0f - 32.0f,
//                //                   floorf((newPos.y() + 32.0f) / 64.0f) * 64.0f - 32.0f);
//                //return newGridPos;
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
//
//void GraphConnectionGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    update();
//    QGraphicsItem::mousePressEvent(event);
//}
//
////----------------------------------------------------------------------------------------
//
//void GraphConnectionGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    // Consider the next time we move the same node as a new undo command
//    // (do not test for left button, it does not seem to work)
//    sMouseClickID++;
//
//    update();
//    QGraphicsItem::mouseReleaseEvent(event);
//}
//
////----------------------------------------------------------------------------------------
//
//void GraphConnectionGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
//{
//    update();
//    QGraphicsItem::mouseDoubleClickEvent(event);
//    //*****emit(DoubleClicked(mBlockProxy));
//}

