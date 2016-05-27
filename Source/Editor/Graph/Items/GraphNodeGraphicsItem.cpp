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
#include "Graph/Items/GraphGraphicsItemDefs.h"
#include "Graph/Items/GraphNodeInputGraphicsItem.h"
#include "Graph/Items/GraphNodeOutputGraphicsItem.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>


unsigned int GraphNodeGraphicsItem::sCurrentNodeID = 0;
unsigned int GraphNodeGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem::GraphNodeGraphicsItem(const QString& title,
                                             QGraphicsScene* scene,
                                             QUndoStack* undoStack)
:   QGraphicsObject()
,   mTitle(title)
,   mUndoStack(undoStack)
{
    // Make the block movable and selectable with the mouse
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the item movements
    setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);

    // Set the depth of the node
    setZValue(GRAPHITEM_NODE_Z_VALUE);

    // Create the output item
    mOutput.item = new GraphNodeOutputGraphicsItem(scene, undoStack);
    scene->addItem(mOutput.item);
    mOutput.item->setParentItem(this);
    mOutput.item->setPos(GRAPHITEM_NODE_WIDTHF + GRAPHITEM_OUTPUT_OFFSET_XF,
                           GRAPHITEM_OUTPUT_REFERENCE_YF
                         + GRAPHITEM_OUTPUT_ROW_MARGIN_YF);

    //! Assign a unique ID to the block for merging undo commands when moving the block
    mNodeID = sCurrentNodeID++;
}

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem::~GraphNodeGraphicsItem()
{
}

//----------------------------------------------------------------------------------------

void GraphNodeGraphicsItem::AddInput(const QString& name)
{
    const unsigned int inputIndex = GetNumInputs();

    // Create the new input description
    Input newInput;
    newInput.name = name;

    // Create the graphics item for the input connector
    newInput.item = new GraphNodeInputGraphicsItem(scene(), mUndoStack);
    scene()->addItem(newInput.item);
    newInput.item->setParentItem(this);
    newInput.item->setPos(GRAPHITEM_INPUT_OFFSET_XF,
                            GRAPHITEM_INPUT_REFERENCE_YF
                          + inputIndex * GRAPHITEM_INPUT_ROW_HEIGHTF
                          + GRAPHITEM_INPUT_ROW_MARGIN_YF);

    // Create the text label next to the input connector
    newInput.labelItem = scene()->addSimpleText(newInput.name);
    newInput.labelItem->setParentItem(this);
    newInput.labelItem->setPos(GRAPHITEM_INPUT_LABEL_OFFSET_XF,
                                 GRAPHITEM_INPUT_REFERENCE_YF
                               + inputIndex * GRAPHITEM_INPUT_ROW_HEIGHTF
                               + GRAPHITEM_INPUT_ROW_MARGIN_YF
                               + GRAPHITEM_INPUT_LABEL_OFFSET_YF);

    mInputs += newInput;

    //! \todo Send signal? Ask for redraw? Avoid at construction time at least
}

//----------------------------------------------------------------------------------------

GraphNodeInputGraphicsItem* GraphNodeGraphicsItem::GetInputItem(unsigned int index) const
{
    if (index < GetNumInputs())
    {
        return mInputs[index].item;
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

QRectF GraphNodeGraphicsItem::boundingRect() const
{
    return QRectF(-GRAPHITEM_INPUT_HALF_WIDTHF + GRAPHITEM_INPUT_OFFSET_XF,
                  0.0f,
                  GRAPHITEM_INPUT_HALF_WIDTHF - GRAPHITEM_INPUT_OFFSET_XF
                  + GRAPHITEM_NODE_WIDTHF
                  + GRAPHITEM_OUTPUT_HALF_WIDTHF + GRAPHITEM_OUTPUT_OFFSET_XF,
                  GRAPHITEM_NODE_HEADER_HEIGHTF + GetBodyHeight() + GRAPHITEM_NODE_FOOTER_HEIGHTF);
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
    painter->setPen(Qt::NoPen);
    //}

    // Set the fill color, darker when selected, lighter when hovered by the mouse
    //QColor fillColor = (option->state & QStyle::State_Selected) ? mBaseColor.darker(150) : mBaseColor;
    //QColor fillColor = Qt::blue;
    //if (option->state & QStyle::State_MouseOver)
    //{
    //    fillColor = fillColor.lighter(125);
    //}
    //painter->setBrush(fillColor);
    painter->setBrush(Qt::NoBrush);

    // Draw the block

    static const QPointF sHeaderPixOrigin (0.0f, 0.0f);
    static const QPointF sBodyTopPixOrigin(0.0f, sHeaderPixOrigin.y()  + GRAPHITEM_NODE_HEADER_HEIGHTF);
    static const QPointF sBodyPixOrigin   (0.0f, sBodyTopPixOrigin.y() + GRAPHITEM_NODE_BODY_TOP_HEIGHTF);

    /*****/QPixmap nodeHeaderPix(":/GraphEditor/NodeHeader128.png");
    painter->drawPixmap(sHeaderPixOrigin, nodeHeaderPix);

    /*****/QPixmap nodeBodyTopPix(":/GraphEditor/NodeBodyTop128.png");
    painter->drawPixmap(sBodyTopPixOrigin, nodeBodyTopPix);

    const float bodyHeight = GetBodyHeight();
    const QSizeF bodyPixSize(GRAPHITEM_NODE_WIDTHF, bodyHeight - GRAPHITEM_NODE_BODY_TOP_HEIGHTF);
    const QRectF sBodyPixRect(sBodyPixOrigin, bodyPixSize);
    /*****/QPixmap nodeBodyPix(":/GraphEditor/NodeBody128.png");
    painter->drawTiledPixmap(sBodyPixRect, nodeBodyPix);

    const QPointF footerPixOrigin(0.0f, sBodyTopPixOrigin.y() + bodyHeight);
    /*****/QPixmap nodeFooterPix(":/GraphEditor/NodeFooter128.png");
    painter->drawPixmap(footerPixOrigin, nodeFooterPix);

    QFont titleFont;
    titleFont.setPointSize(GRAPHITEM_NODE_TITLE_POINT_SIZE);
    titleFont.setWeight(/******/QFont::Bold);
    painter->setFont(titleFont);
    painter->setPen(/******/Qt::white);
    QRectF titleRect(GRAPHITEM_NODE_TITLE_MARGIN_XF,
                     GRAPHITEM_NODE_TITLE_MARGIN_TOPYF,
                     GRAPHITEM_NODE_WIDTHF
                     - 2.0f * GRAPHITEM_NODE_TITLE_MARGIN_XF,
                     GRAPHITEM_NODE_HEADER_HEIGHTF
                     - GRAPHITEM_NODE_TITLE_MARGIN_TOPYF
                     - GRAPHITEM_NODE_TITLE_MARGIN_BOTTOMYF);
    painter->drawText(titleRect, Qt::AlignCenter, mTitle);
}

//----------------------------------------------------------------------------------------

float GraphNodeGraphicsItem::GetBodyHeight() const
{
    // If there is no input, count at least one row for the output connector
    const unsigned int numInputs = (GetNumInputs() >= 1 ? GetNumInputs() : 1);

    // Desired height without accounting for edge cases
    const float desiredHeight =   static_cast<float>(numInputs) * GRAPHITEM_INPUT_ROW_HEIGHTF
                                + GRAPHITEM_INPUT_ROW_MARGIN_YF * 2.0f;

    // If the height is so small we cannot fit the body top image,
    // set the height to the body top image
    if (desiredHeight < GRAPHITEM_NODE_BODY_TOP_HEIGHTF)
    {
        return GRAPHITEM_NODE_BODY_TOP_HEIGHTF;
    }
    else
    {
        return desiredHeight;
    }
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
                /*****/
                //QPointF newPos = value.toPointF();
                //QPointF newGridPos(floorf((newPos.x() + 32.0f) / 64.0f) * 64.0f - 32.0f,
                //                   floorf((newPos.y() + 32.0f) / 64.0f) * 64.0f - 32.0f);
                //return newGridPos;
                /*****/

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

