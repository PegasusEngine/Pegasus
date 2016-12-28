/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeInputGraphicsItem.cpp
//! \author	Karolyn Boulanger
//! \date	10th April 2016
//! \brief	Graphics item representing one node input in the graph

#include "Graph/Items/GraphNodeInputGraphicsItem.h"
#include "Graph/Items/GraphConnectionGraphicsItem.h"
#include "Graph/Items/GraphGraphicsItemDefs.h"

#include <QAbstractScrollArea>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


//unsigned int GraphNodeInputGraphicsItem::sCurrentNodeID = 0;
//unsigned int GraphNodeInputGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

GraphNodeInputGraphicsItem::GraphNodeInputGraphicsItem(QGraphicsScene* scene, QUndoStack* undoStack)
:   QGraphicsObject()
,   mUndoStack(undoStack)
,   mConnectionItem(nullptr)
{
    // Make the block movable and selectable with the mouse
    //setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the item movements
    setFlag(ItemSendsScenePositionChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the input (in front of the nodes)
    setZValue(GRAPHITEM_INPUT_Z_VALUE);

//    QPen pathPen;
//    pathPen.setWidth(4);
//    QColor pathPenColor(170, 198, 198, 255);
//    pathPen.setColor(pathPenColor);
//    QPainterPath path;
//    path.moveTo(-200.0f, 200.0f);
//    path.cubicTo(-150.0f, 200.0f, -48.0f, 44.0f, 2.0f, 44.0f);
//    QGraphicsPathItem* pathItem = scene->addPath(path, pathPen, Qt::NoBrush);
//    pathItem->setParentItem(this);
//    QPainterPath path2;
//    path2.moveTo(-100.0f, 400.0f);
//    path2.cubicTo(-50.0f, 400.0f, -48.0f, 60.0f, 2.0f, 60.0f);
//    QGraphicsPathItem* pathItem2 = scene->addPath(path2, pathPen, Qt::NoBrush);
//    pathItem2->setParentItem(this);
//    QPainterPath path3;
//    path3.moveTo(128.0f, 44.0f);
//    path3.cubicTo(178.0f, 44.0f, 250.0f, 160.0f, 300.0f, 160.0f);
//    QGraphicsPathItem* pathItem3 = scene->addPath(path3, pathPen, Qt::NoBrush);
//    pathItem3->setParentItem(this);
//
//    QPen inputPen;
//    inputPen.setWidth(2);
//    QLinearGradient operatorGradient(0.0f, 32.0f, 0.0f, 64.0f);
//    operatorGradient.setColorAt(0, QColor(100, 230, 230, 255));
//    operatorGradient.setColorAt(1, QColor(100, 230, 230, 255).darker(120));
//    QBrush operatorBrush(operatorGradient);
////    QGraphicsEllipseItem * ellipse = scene->addEllipse(0.0f, 24.0f, 8.0f, 8.0f, inputPen, operatorBrush);
//    QGraphicsPixmapItem * ellipse = scene->addPixmap(QPixmap(":/GraphEditor/NodeInputIdle16.png"));
//    ellipse->setParentItem(this);
//    ellipse->setPos(-4.0f, 36.0f);
//    QGraphicsSimpleTextItem * text1 = scene->addSimpleText("Item1");
//    text1->setPos(16.0f, 38.0f);
//    text1->setParentItem(this);
//    //QGraphicsEllipseItem * ellipse2 = scene->addEllipse(0.0f, 40.0f, 8.0f, 8.0f, inputPen, operatorBrush);
//    QGraphicsPixmapItem * ellipse2 = scene->addPixmap(QPixmap(":/GraphEditor/NodeInputIdle16.png"));
//    ellipse2->setParentItem(this);
//    ellipse2->setPos(-4.0f, 52.0f);
//    QGraphicsSimpleTextItem * text2 = scene->addSimpleText("Item2");
//    text2->setPos(16.0f, 54.0f);
//    text2->setParentItem(this);
//    QGraphicsPixmapItem * ellipse3 = scene->addPixmap(QPixmap(":/GraphEditor/NodeInputIdle16.png"));
//    ellipse3->setParentItem(this);
//    ellipse3->setPos(116.0f, 36.0f);

    //! Assign a unique ID to the block for merging undo commands when moving the block
  //  mNodeID = sCurrentNodeID++;
}

//----------------------------------------------------------------------------------------

GraphNodeInputGraphicsItem::~GraphNodeInputGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void GraphNodeInputGraphicsItem::SetConnection(GraphConnectionGraphicsItem* connectionItem)
{
    if ((connectionItem != nullptr) && (mConnectionItem != nullptr))
    {
        ED_FAILSTR("Cannot yet create a connection to a node input when there is already one.");
    }
    else
    {
        mConnectionItem = connectionItem;
    }
}

//----------------------------------------------------------------------------------------

QRectF GraphNodeInputGraphicsItem::boundingRect() const
{
    return QRectF(-GRAPHITEM_INPUT_HALF_WIDTHF, -GRAPHITEM_INPUT_HALF_WIDTHF,
                  GRAPHITEM_INPUT_WIDTHF, GRAPHITEM_INPUT_WIDTHF);
}

//----------------------------------------------------------------------------------------

void GraphNodeInputGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::NoBrush);

    static const QPointF sPixOrigin(-GRAPHITEM_INPUT_HALF_WIDTHF, -GRAPHITEM_INPUT_HALF_WIDTHF);

    bool isSelected = (option->state & QStyle::State_Selected) != 0;
    bool isHovering = (option->state & QStyle::State_MouseOver) != 0;

    /*****/QPixmap nodeInputPix(isSelected ? ":/GraphEditor/NodeInputSelect16.png" : (isHovering ? ":/GraphEditor/NodeInputHover16.png" : ":/GraphEditor/NodeInputIdle16.png"));
    painter->drawPixmap(sPixOrigin, /****/nodeInputPix);
}

//----------------------------------------------------------------------------------------

QVariant GraphNodeInputGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    switch (change)
    {
        // Called when the position of the node output changes by even one pixel
        // (using scene position updates, since the position relative to the parent node
        // does not change)
        case ItemScenePositionHasChanged:
            {
                // Trigger a redraw of the connection
                if (mConnectionItem != nullptr)
                {
                    mConnectionItem->AdjustGeometry();
                }

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

void GraphNodeInputGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    update();
    emit StartFloatingSrcConnection(this, mouseEvent);
    mouseEvent->accept();

    QGraphicsItem::mousePressEvent(mouseEvent);
}

//----------------------------------------------------------------------------------------

void GraphNodeInputGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    // Consider the next time we move the same node as a new undo command
    // (do not test for left button, it does not seem to work)
    //sMouseClickID++;

    update();
    emit EndFloatingSrcConnection(this, mouseEvent);
    mouseEvent->accept();

    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

//----------------------------------------------------------------------------------------

void GraphNodeInputGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    update();
    QGraphicsItem::mouseDoubleClickEvent(mouseEvent);
    //*****emit(DoubleClicked(mBlockProxy));
}

