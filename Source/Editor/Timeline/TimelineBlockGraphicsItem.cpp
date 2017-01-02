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
#include "Pegasus/PegasusAssetTypes.h"
#include <QAbstractScrollArea>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QUndoStack>
#include <QMenu>
#include <QAction>


//! Depth of the block when drawn. Positive since it needs to be rendered in front of the grid at least
#define TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE                2.0f

unsigned int TimelineBlockGraphicsItem::sCurrentBlockID = 0;
unsigned int TimelineBlockGraphicsItem::sMouseClickID = 0;

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::TimelineBlockGraphicsItem(const ShadowBlockState& blockState,
                                                     const ShadowTimelineState* timelineState,
                                                     unsigned int lane,
                                                     float horizontalScale,
                                                     QUndoStack* undoStack)
:   QGraphicsObject(),
    mBlockState(blockState),
    mTimelineState(timelineState),
    mEnableUndo(true),
    mIsMoving(false),
    mUndoStack(undoStack)
{

    if (horizontalScale <= 0.0f)
    {
        ED_FAILSTR("Invalid horizontal scale (%f) for the timeline graphics item. It should be > 0.0f.", horizontalScale);
        mHorizontalScale = 1.0f;
    }
    else
    {
        mHorizontalScale = horizontalScale;
    }

    // Make the block movable and selectable with the mouse
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);

    // Enable the itemChange() callback, to receive notifications about the block movements
    setFlag(ItemSendsGeometryChanges);

    // Enable mouse hovering state for the paint() function
    setAcceptHoverEvents(true);

    // Caching performed at paint device level, best quality and lower memory usage
    setCacheMode(DeviceCoordinateCache);
    SetLane(lane);
    // Set the depth of the block (positive since it needs to be rendered
    // in front of the grid at least)
    setZValue(TIMELINE_BLOCK_GRAPHICS_ITEM_Z_VALUE);

    mCtxMenu = new QMenu();
    mChangeScriptAction = mCtxMenu->addAction(tr(""));

    connect(mChangeScriptAction, SIGNAL(triggered()),
            this, SLOT(CtxMenuChangeScript()));
    QAction* removeScriptAction = mCtxMenu->addAction(tr("remove script"));
    connect (removeScriptAction, SIGNAL(triggered()),
             this, SLOT(CtxRemoveScript()));

    mNoScriptCtxMenu = new QMenu();
    QAction* attachScriptAction = mNoScriptCtxMenu->addAction(tr("attach script"));
    connect (attachScriptAction, SIGNAL(triggered()),
            this, SLOT(CtxMenuChangeScript()));
    
    FlushVisualProperties();

    mScriptIcon = QIcon(tr(Pegasus::ASSET_TYPE_BLOCKSCRIPT.mIconPath));
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::FlushVisualProperties()
{
    // invalidate and redraw
    update(boundingRect());

    // Set the block color
    unsigned col = mBlockState.GetColor();
    mBaseColor = QColor((col >> 24) & 0xFF, (col >> 16) & 0xFF, (col >> 8) & 0xFF);

    // Set the initial position and duration, and update the scaled position and length
    SetDuration(mBlockState.GetDuration(), false);
    SetBeat(mBlockState.GetBeat());

    mName = mBlockState.GetName();
    mClassName = mBlockState.GetClassName();

}

//----------------------------------------------------------------------------------------

TimelineBlockGraphicsItem::~TimelineBlockGraphicsItem()
{
    delete (mCtxMenu);
    delete (mNoScriptCtxMenu);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLane(unsigned int lane)
{
    mEnableUndo = false;

    mLane = lane;

    // Update the vertical position
    SetYFromLane();
    setPos(mX, mY);
    update(boundingRect());
    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetBeat(Pegasus::Timeline::Beat beat, bool updateItem)
{
    mEnableUndo = false;

    mBlockState.GetRootState().insert(ShadowBlockState::Str(ShadowBlockState::PROP_BLOCK_BEAT), QVariant((unsigned)beat));
    mBeat = beat;

    // Update the scaled position
    SetXFromBeat();
    setPos(mX, mY);
    if (updateItem)
    {
        update(boundingRect());
    }
    mEnableUndo = true;
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetDuration(Pegasus::Timeline::Duration duration, bool updateItem)
{
    if (duration == 0)
    {
        ED_LOG("Invalid duration of 0 for the timeline graphics item. It should be > 0.");
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
    painter->drawText(textRect, mName.isEmpty() ? mClassName :  mName + ":" + mClassName);


    QRect rect(mLength - 16, 0, 16, 16);
    fillColor = fillColor.lighter(160);
    painter->setBrush(fillColor);
    painter->drawRect(rect);
    
    if (mBlockState.HasBlockScript())
    {
        mScriptIcon.paint(painter, rect);
    }
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
                if (!mPositionQueue.empty())
                {
                    QPointF candidatePoint = mPositionQueue.front();
                    mX = candidatePoint.x();
                    mY = candidatePoint.y();
                    mPositionQueue.pop_front();
                }
                return QPointF(mX,mY);
            }
        default:
            break;
    };
    return QGraphicsItem::itemChange(change, value);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene * const parentScene = scene();
    QList<QGraphicsItem *> selectedItems;
    mInitialMouse = QPointF(mX,mY)+event->pos();
    mInitialPoint = QPointF(mX,mY);
    if (parentScene != nullptr)
    {
        mIsMoving = true;        
        selectedItems = parentScene->selectedItems();
        for (int i = 0; i < selectedItems.size(); ++i)
        {
            TimelineBlockGraphicsItem* blockGraphicsItem = static_cast<TimelineBlockGraphicsItem*>(selectedItems[i]);                
            
            blockGraphicsItem->mInitialPoint = QPointF(blockGraphicsItem->mX,blockGraphicsItem->mY);
        }
    }
    update();
    QGraphicsItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIsMoving)
    {
        QGraphicsScene * const parentScene = scene();
        QList<QGraphicsItem *> selectedItems;
        bool requestDrawAllViewports = false;
        
        if (parentScene != nullptr)
        {
            QPointF delta = event->pos()+QPointF(mX,mY)-mInitialMouse;
            selectedItems = parentScene->selectedItems();
            for (int i = 0; i < selectedItems.size(); ++i)
            {
                requestDrawAllViewports = true;
                TimelineBlockGraphicsItem* blockGraphicsItem = static_cast<TimelineBlockGraphicsItem*>(selectedItems[i]);                
                emit RequestBlockMove(blockGraphicsItem, QPointF(blockGraphicsItem->mInitialPoint.x(), blockGraphicsItem->mInitialPoint.y()) + delta);
            }
        }

        static const int MAX_PER_FRAME_THROTTLE = 6;
        static int sCurrentPerFrameThrottleRequest = 0;

        if (requestDrawAllViewports)
        {
            //Crappy but pretty elegant hack:
            // in order to not oversaturate and send every single time the mouse move event fires a frame,
            // we just send it every MAX_PER_FRAME_THROTTLE events the mouse moves. This improves behaviour and makes it possible to not have
            // an outrageous lag when we move a set of timelines.
            if ((sCurrentPerFrameThrottleRequest++ % MAX_PER_FRAME_THROTTLE) == 0)
            {
                emit RequestDrawAllViewports();
            }
        }
        
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Consider the next time we move the same block as a new undo command
    // (do not test for left button, it does not seem to work)
    sMouseClickID++;

    update();
    QGraphicsItem::mouseReleaseEvent(event);

    if (event->pos().x() <= qreal(mLength) && event->pos().x() >= (mLength - 16))
    {
        if (!mBlockState.HasBlockScript())
        {
            mNoScriptCtxMenu->exec(event->screenPos());
        }
        else
        {
            mChangeScriptAction->setText(tr("<") + mBlockState.GetBlockScriptPath() + tr(">"));
            mCtxMenu->exec(event->screenPos());
        }
    }
    mIsMoving = false;
    emit RequestDrawAllViewports();
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::UIUpdateBlockScript(const QString& script)
{
    //update the shadow state, purely for UI purposes.
    mBlockState.SetScript(script);
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::UIClearBlockScript()
{
    //update the shadow state, purely for UI purposes.
    mBlockState.RemoveScript();
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::CtxMenuChangeScript()
{
    emit(RequestChangeScript(this, mBlockState.GetGuid()));    
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::CtxRemoveScript()
{
    emit(RequestRemoveScript(this, mBlockState.GetGuid()));
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseDoubleClickEvent(event);
    if (mBlockState.HasBlockScript())
    {
        emit(DoubleClicked(mBlockState.GetBlockScriptPath()));
    }
}

//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetXFromBeat()
{
    mX = (mBeat * 1.0f/((float)mTimelineState->GetNumTicksPerBeat())) * mHorizontalScale * TIMELINE_BEAT_WIDTH;
}

//----------------------------------------------------------------------------------------

Pegasus::Timeline::Beat TimelineBlockGraphicsItem::GetBeatFromX(float x) const
{
    //TODO: use the cached ticks per beat if this becomes expensive.
    float ticksPerBeatFloat = static_cast<float>(mTimelineState->GetNumTicksPerBeat());
    int beat = static_cast<int>(floor((x / (mHorizontalScale * TIMELINE_BEAT_WIDTH)) * ticksPerBeatFloat));
    if (beat < 0)
    {
        beat = 0;
    }

    return static_cast<Pegasus::Timeline::Beat>(beat);
}

//----------------------------------------------------------------------------------------


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
    const int numLanes = static_cast<int>(mTimelineState->GetLaneCount());
    if (lane >= numLanes)
    {
        lane = numLanes - 1;
    }

    return static_cast<unsigned int>(lane);
}


//----------------------------------------------------------------------------------------

void TimelineBlockGraphicsItem::SetLengthFromDuration()
{

    mLength = (mDuration * 1.0f/(float)(mTimelineState->GetNumTicksPerBeat())) * (mHorizontalScale * TIMELINE_BEAT_WIDTH);

}

