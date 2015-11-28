/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineGraphicsView.cpp
//! \author	Karolyn Boulanger
//! \date	10th June 2013
//! \brief	Graphics view containing the graphics timeline representation

#include "Timeline/TimelineGraphicsView.h"
#include "Timeline/TimelineBackgroundBeatGraphicsItem.h"
#include "Timeline/TimelineBackgroundBeatLineGraphicsItem.h"
#include "Timeline/TimelineLaneHeaderGraphicsItem.h"
#include "Timeline/TimelineBlockGraphicsItem.h"
#include "Timeline/TimelineCursorGraphicsItem.h"
#include "Timeline/TimelineSizes.h"

#include "Application/Application.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"

#include <QWheelEvent>


TimelineGraphicsView::TimelineGraphicsView(QWidget *parent)
:   QGraphicsView(parent),
    mRightClickCursorDraggingEnabled(true)
{
    // Set the scrollbars to be always visible
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Create the scene containing the items to render
    QGraphicsScene * scene = new QGraphicsScene(this);
    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(SelectionChanged()));
    
    // Set the indexing method of the items.
    // NoIndex is supposedly faster when items move a lot.
    // BspTreeIndex is faster for static scenes.
    // Adapt based on the performances.
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // Set the graphics view to the created scene
    setScene(scene);

    // Set the cache mode
    setCacheMode(CacheBackground);

    // Set the update mode, when updating the content of the view
    //! \todo Change based on the performances
    setViewportUpdateMode(BoundingRectViewportUpdate);

    // Disable anti-aliasing (default value before reading from the settings)
    // but enable text antialiasing all the time
    EnableAntialiasing(false);
    setRenderHint(QPainter::TextAntialiasing);

    // Use the mouse position to focus the zoom
    setTransformationAnchor(AnchorUnderMouse);

    // Enable the selection of blocks using a drag box
    setDragMode(QGraphicsView::RubberBandDrag);

    // Initialize members to default value, but do not create the default lane,
    // this will be done by a call to Initialize()
    Clear();
}

//----------------------------------------------------------------------------------------

TimelineGraphicsView::~TimelineGraphicsView()
{
    Clear();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::EnableAntialiasing(bool enable)
{
    setRenderHint(QPainter::Antialiasing, enable);

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Update every items to take antialiasing into account
    foreach (QGraphicsItem *item, scene()->items())
    {
        item->update();
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::RefreshFromTimeline()
{
    Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        // Application running

        Pegasus::Timeline::ITimelineManagerProxy * const timeline = application->GetTimelineProxy();

        // Handle the length of the timeline
        const unsigned int numBeats = timeline->GetCurrentTimeline()->GetNumBeats();
        if (numBeats < mNumBeats)
        {
            // Remove the background items at the end of the timeline
            RemoveBackgroundGraphicsItems(numBeats, mNumBeats - numBeats);
            mNumBeats = numBeats;
        }
        else if (numBeats > mNumBeats)
        {
            // Add the missing background items at the end of the timeline
            CreateBackgroundGraphicsItems(mNumBeats, numBeats - mNumBeats);
            mNumBeats = numBeats;
        }

        // Handle the number of lanes of the timeline
        const unsigned int numLanes = timeline->GetCurrentTimeline()->GetNumLanes();
        if (numLanes < mNumLanes)
        {
            // Remove the extra lanes from the end of the list
            RemoveLanes(numLanes, mNumLanes - numLanes);
            mNumLanes = numLanes;
        }
        else if (numLanes > mNumLanes)
        {
            // Add the missing lanes at the end of the list
            CreateLanes(mNumLanes, numLanes - mNumLanes);
            mNumLanes = numLanes;
        }

        // Refresh the content of every lane
        for (unsigned int l = 0; l < numLanes; ++l)
        {
            RefreshLaneFromTimelineLane(l, timeline->GetCurrentTimeline()->GetLane(l));
        }
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::Initialize()
{
    // Clear the content of the timeline
    Clear();

    ED_LOG("Creating the default lane in the timeline graphics view");

    // Create the cursor
    mCursorItem = new TimelineCursorGraphicsItem(1, mHorizontalScale);
    scene()->addItem(mCursorItem);

    // Create the default lane
    CreateLanes(0, 1);
    CreateBackgroundGraphicsItems(0, mNumBeats);

    // Update the scene bounding box for the default lane
    UpdateSceneRect();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::AddLane()
{
    //! \todo Add a lane in the Pegasus timeline

    // Create a new lane in the graphics view
    CreateLanes(mNumLanes, 1);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::SetCursorFromBeat(float beat)
{
    //! \todo Handle out-of-bounds at the end of the timeline

    if (mCursorItem != nullptr)
    {
        // Reposition the cursor
        mCursorItem->SetBeat(beat);

        // Make sure cursor is visible, scroll otherwise
        // (100 pixels horizontal margin, default vertical margin)
        ensureVisible(mCursorItem, 100, 50);
    }
    else
    {
        ED_FAIL();
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::OnPlayModeToggled(bool enable)
{
    mRightClickCursorDraggingEnabled = !enable;
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::SelectionChanged()
{
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (selectedItems.size() == 0)
    {
        emit BlocksDeselected();
    }
    else if (selectedItems.size() == 1)
    {
        TimelineBlockGraphicsItem * item = static_cast<TimelineBlockGraphicsItem *>(selectedItems[0]);
        Pegasus::Timeline::IBlockProxy * proxy = item->GetBlockProxy();
        emit BlockSelected(proxy);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::SetHorizontalScale(float scale)
{
    ED_ASSERTSTR((scale >= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MIN) && (scale <= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MAX),
                 "Invalid horizontal scale (%f) for the timeline graphics view.", scale);
    mHorizontalScale = scale;

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Update the horizontal scale of all the objects that depend on the horizontal scale
    // (this invalidates the cache of the graphics items)
    foreach (TimelineBackgroundBeatGraphicsItem * backgroundBeatItem, mBackgroundBeatItems)
    {
        ED_ASSERTSTR(backgroundBeatItem != nullptr, "Invalid item in the list of background beat graphics items");
        backgroundBeatItem->SetHorizontalScale(mHorizontalScale);
    }
    foreach (TimelineBackgroundBeatLineGraphicsItem * backgroundBeatLineItem, mBackgroundBeatLineItems)
    {
        ED_ASSERTSTR(backgroundBeatLineItem != nullptr, "Invalid item in the list of background beat line graphics items");
        backgroundBeatLineItem->SetHorizontalScale(mHorizontalScale);
    }
    for (unsigned int l = 0; l < mNumLanes; ++l)
    {
        QMapIterator<Pegasus::Timeline::IBlockProxy *, TimelineBlockGraphicsItem *> it(mBlockItems[l]);
        while (it.hasNext())
        {
            it.next();
            it.value()->SetHorizontalScale(mHorizontalScale);
        }
    }
    ED_ASSERTSTR(mCursorItem != nullptr, "Invalid graphics item for the cursor");
    mCursorItem->SetHorizontalScale(mHorizontalScale);
    
    // Update the bounding box of the scene
    UpdateSceneRect();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::MultiplyHorizontalScale(float scaleFactor)
{
    if (scaleFactor <= 0.0f)
    {
        ED_FAILSTR("Invalid horizontal scale factor (%f) for the timeline graphics view. It should be > 0.0f.", scaleFactor);
        return;
    }

    const float newHorizontalScale = scaleFactor * mHorizontalScale;
    if (   (newHorizontalScale >= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MIN)
        && (newHorizontalScale <= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MAX))
    {
        // Range valid. Apply the scale and redraw the view
        SetHorizontalScale(newHorizontalScale);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::SetZoom(float zoom)
{
    if (zoom < TIMELINE_GRAPHICS_VIEW_ZOOM_MIN)
    {
        zoom = TIMELINE_GRAPHICS_VIEW_ZOOM_MIN;
        ED_FAILSTR("Invalid zoom factor (%f) for the timeline graphics view. It should be >= %f.",
                   zoom, TIMELINE_GRAPHICS_VIEW_ZOOM_MIN);
    }
    else if (zoom > TIMELINE_GRAPHICS_VIEW_ZOOM_MAX)
    {
        zoom = TIMELINE_GRAPHICS_VIEW_ZOOM_MAX;
        ED_FAILSTR("Invalid zoom factor (%f) for the timeline graphics view. It should be <= %f.",
                   zoom, TIMELINE_GRAPHICS_VIEW_ZOOM_MAX);
    }

    mZoom = zoom;

    // Apply the new zoom level to the view
    QTransform newTransform;
    newTransform.scale(mZoom, mZoom);
    setTransform(newTransform);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::MultiplyZoom(float zoomFactor)
{
    if (zoomFactor <= 0.0f)
    {
        ED_FAILSTR("Invalid zoom multiplier (%f) for the timeline graphics view. It should be > 0.0f.", zoomFactor);
        return;
    }

    const float newZoom = zoomFactor * mZoom;
    if (   (newZoom >= TIMELINE_GRAPHICS_VIEW_ZOOM_MIN)
        && (newZoom <= TIMELINE_GRAPHICS_VIEW_ZOOM_MAX))
    {
        // Range valid. Apply the zoom and redraw the view
        SetZoom(newZoom);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
        case Qt::Key_Home:
            emit BeatUpdated(0.0f);
            break;

        default:
            QGraphicsView::keyPressEvent(event);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        // When right-clicking, move the cursor
        SetBeatFromMouse(event);
    }
    else
    {
        // Regular mouse handling
        QGraphicsView::mousePressEvent(event);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        if (mRightClickCursorDraggingEnabled)
        {
            // When right-dragging, move the cursor
            SetBeatFromMouse(event);
        }
    }
    else
    {
        // Regular mouse handling
        QGraphicsView::mouseMoveEvent(event);
    }
}

//----------------------------------------------------------------------------------------

#ifndef QT_NO_WHEELEVENT
void TimelineGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        // Global zoom
        MultiplyZoom(pow(2.0, event->delta() / 240.0));
    }
    else
    {
        MultiplyHorizontalScale(pow(2.0, event->delta() / 240.0));
    }
}
#endif

//----------------------------------------------------------------------------------------

//void TimelineGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
//{
//    Q_UNUSED(rect);
//    
//    //// Shadow
//    //QRectF sceneRect = this->sceneRect();
//    //QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
//    //QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
//    //if (rightShadow.intersects(rect) || rightShadow.contains(rect))
//    //    painter->fillRect(rightShadow, Qt::darkGray);
//    //if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
//    //    painter->fillRect(bottomShadow, Qt::darkGray);
//
//    //// Fill
//    //QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
//    //gradient.setColorAt(0, Qt::white);
//    //gradient.setColorAt(1, Qt::lightGray);
//    //painter->fillRect(rect.intersected(sceneRect), gradient);
//    //painter->setBrush(Qt::NoBrush);
//    //painter->drawRect(sceneRect);
//}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::Clear()
{
    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Destroy the cached lists of items
    mBackgroundBeatItems.clear();
    mBackgroundBeatLineItems.clear();
    mLaneHeaderItems.clear();
    mBlockItems.clear();
    mCursorItem = nullptr;

    // Set the default member values
    mNumBeats = 32;
    mNumLanes = 0;
    mHorizontalScale = 1.0f;
    mZoom = 1.0f;

    // Clear the list of items in the graphics view
    scene()->clear();
    SetZoom(mZoom);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::UpdateSceneRect()
{
    scene()->setSceneRect(-TIMELINE_LANE_HEADER_WIDTH,
                          -TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT,
                          TIMELINE_LANE_HEADER_WIDTH + static_cast<float>(mNumBeats) * TIMELINE_BEAT_WIDTH * mHorizontalScale,
                          TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT + static_cast<float>(mNumLanes) * TIMELINE_LANE_HEIGHT + TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::CreateLanes(unsigned int firstLane, unsigned int numLanes)
{
    ED_ASSERTSTR(firstLane <= mNumLanes, "Invalid first lane (%d), it should be <= %d", firstLane, mNumLanes);
    ED_ASSERTSTR(numLanes >= 1, "Invalid number of lanes to add (%d), it should be >= 1", numLanes);
    const unsigned int lastLane = firstLane + numLanes;

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Create the lane specific graphics items
    unsigned int l;
    for (l = firstLane; l < lastLane; ++l)
    {
        // Add a lane header
        TimelineLaneHeaderGraphicsItem * laneHeaderItem = new TimelineLaneHeaderGraphicsItem(l);
        scene()->addItem(laneHeaderItem);
        mLaneHeaderItems += laneHeaderItem;

        // Create the empty list of block items
        mBlockItems += LaneBlockList();
    }

    // Set the new number of lanes
    mNumLanes += numLanes;

    // Update the elements that depend on the number of lanes
    // (this invalidates the cache of the block graphics items)
    foreach (TimelineBackgroundBeatGraphicsItem * backgroundBeatItem, mBackgroundBeatItems)
    {
        ED_ASSERTSTR(backgroundBeatItem != nullptr, "Invalid item in the list of background beat graphics items");
        backgroundBeatItem->SetNumLanes(mNumLanes);
    }
    foreach (TimelineBackgroundBeatLineGraphicsItem * backgroundBeatLineItem, mBackgroundBeatLineItems)
    {
        ED_ASSERTSTR(backgroundBeatLineItem != nullptr, "Invalid item in the list of background beat line graphics items");
        backgroundBeatLineItem->SetNumLanes(mNumLanes);
    }
    ED_ASSERTSTR(mCursorItem != nullptr, "Invalid graphics item for the cursor");
    mCursorItem->SetNumLanes(mNumLanes);

    // Update the bounding box of the scene
    UpdateSceneRect();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::RemoveLanes(unsigned int firstLane, unsigned int numLanes)
{
    ED_ASSERTSTR(firstLane <= mNumLanes, "Invalid first lane (%d), it should be <= %d", firstLane, mNumLanes);
    ED_ASSERTSTR(numLanes >= 1, "Invalid number of lanes to remove (%d), it should be >= 1", numLanes);
    ED_ASSERTSTR(numLanes <= mNumLanes - firstLane, "Invalid number of lanes to remove (%d), it should be <= %d", numLanes, mNumLanes - firstLane);
    const int lastLane = static_cast<int>(firstLane + numLanes);

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Remove the lane specific graphics items
    for (int l = lastLane - 1; l >= static_cast<int>(firstLane); --l)
    {
        //! Remove the content of the lane
        ClearBlockItems(l);
        mBlockItems.removeAt(l);

        // Remove the lane header
        scene()->removeItem(mLaneHeaderItems.takeAt(l));
    }

    // Set the new number of lanes
    mNumLanes -= numLanes;

    // Update the elements that depend on the number of lanes
    // (this invalidates the cache of the block graphics items)
    foreach (TimelineBackgroundBeatGraphicsItem * backgroundBeatItem, mBackgroundBeatItems)
    {
        ED_ASSERTSTR(backgroundBeatItem != nullptr, "Invalid item in the list of background beat graphics items");
        backgroundBeatItem->SetNumLanes(mNumLanes);
    }
    foreach (TimelineBackgroundBeatLineGraphicsItem * backgroundBeatLineItem, mBackgroundBeatLineItems)
    {
        ED_ASSERTSTR(backgroundBeatLineItem != nullptr, "Invalid item in the list of background beat line graphics items");
        backgroundBeatLineItem->SetNumLanes(mNumLanes);
    }
    ED_ASSERTSTR(mCursorItem != nullptr, "Invalid graphics item for the cursor");
    mCursorItem->SetNumLanes(mNumLanes);

    // Update the bounding box of the scene
    UpdateSceneRect();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::CreateBackgroundGraphicsItems(unsigned int firstBeat, unsigned int numBeats)
{
    ED_ASSERTSTR(firstBeat <= mNumBeats, "Invalid first beat (%d), it should be <= %d", firstBeat, mNumBeats);
    ED_ASSERTSTR(numBeats >= 1, "Invalid number of beats to add (%d), it should be >= 1", numBeats);
    const unsigned int lastBeat = firstBeat + numBeats;

    for (unsigned int b = firstBeat; b < lastBeat; ++b)
    {
        TimelineBackgroundBeatGraphicsItem * beatItem = new TimelineBackgroundBeatGraphicsItem(b, mNumLanes, mHorizontalScale);
        scene()->addItem(beatItem);
        mBackgroundBeatItems += beatItem;

        TimelineBackgroundBeatLineGraphicsItem * beatLineItem = new TimelineBackgroundBeatLineGraphicsItem(b, mNumLanes, mHorizontalScale);
        scene()->addItem(beatLineItem);
        mBackgroundBeatLineItems += beatLineItem;
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::RemoveBackgroundGraphicsItems(unsigned int firstBeat, unsigned int numBeats)
{
    ED_ASSERTSTR(firstBeat <= mNumBeats, "Invalid first beat (%d), it should be <= %d", firstBeat, mNumBeats);
    ED_ASSERTSTR(numBeats >= 1, "Invalid number of beats to remove (%d), it should be >= 1", numBeats);
    ED_ASSERTSTR(numBeats <= mNumBeats - firstBeat, "Invalid number of beats to remove (%d), it should be <= %d", numBeats, mNumBeats - firstBeat);
    const int lastBeat = static_cast<int>(firstBeat + numBeats);

    //! \todo Remove the content of the lane first? What about the blocks that are outside the range?

    for (int b = lastBeat - 1; b >= static_cast<int>(firstBeat); --b)
    {
        // Remove the graphics items
        scene()->removeItem(mBackgroundBeatItems.takeAt(b));
        scene()->removeItem(mBackgroundBeatLineItems.takeAt(b));
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::ClearBlockItems(unsigned int laneIndex)
{
    ED_ASSERTSTR(laneIndex < static_cast<unsigned int>(mBlockItems.size()), "Invalid block items list index (%d), it should be < %d", laneIndex, mBlockItems.size());

    // Clear the list of block items for the lane
    QMapIterator<Pegasus::Timeline::IBlockProxy *, TimelineBlockGraphicsItem *> it(mBlockItems[laneIndex]);
    while (it.hasNext())
    {
        it.next();
        scene()->removeItem(it.value());
    }
    mBlockItems[laneIndex].clear();
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::RefreshLaneFromTimelineLane(unsigned int laneIndex, const Pegasus::Timeline::ILaneProxy * laneProxy)
{
    if (laneProxy != nullptr)
    {
        // Perform a full refresh of the lane rather than an incremental one
        ClearBlockItems(laneIndex);

        // Update the name of the lane
        if (laneProxy->IsNameDefined())
        {
            mLaneHeaderItems[laneIndex]->SetName(laneProxy->GetName());
        }
        else
        {
            mLaneHeaderItems[laneIndex]->SetDefaultName();
        }

        // Get the list of blocks in the timeline
        Pegasus::Timeline::IBlockProxy ** blockProxies = new Pegasus::Timeline::IBlockProxy * [Pegasus::Timeline::LANE_MAX_NUM_BLOCKS];
        const unsigned int numBlocks = laneProxy->GetBlocks(blockProxies);

        // For each block, create a corresponding graphics item
        for (unsigned int b = 0; b < numBlocks; ++b)
        {
            Pegasus::Timeline::IBlockProxy * blockProxy = blockProxies[b];
            if (blockProxy != nullptr)
            {
                TimelineBlockGraphicsItem * item = new TimelineBlockGraphicsItem(blockProxy,
                                                                                 laneIndex,
                                                                                 mHorizontalScale,
                                                                                 GetUndoStack());
                scene()->addItem(item);
                mBlockItems[laneIndex].insert(blockProxy, item);

                // Connect the block moved signal to a request to re-render the viewport
                connect(item, SIGNAL(BlockMoved()),
                        this, SIGNAL(BlockMoved()));

                // Connect the block moved signal to request a double click action 
                connect(item, SIGNAL(DoubleClicked(Pegasus::Timeline::IBlockProxy*)),
                        this, SIGNAL(BlockDoubleClicked(Pegasus::Timeline::IBlockProxy*)));
            }
            else
            {
                ED_FAILSTR("Unable to get the block %d of lane %d, it is a nullptr", b, laneIndex);
            }
        }

        delete[] blockProxies;
    }
    else
    {
        ED_FAILSTR("Unable to refresh the lane %d because it is nullptr", laneIndex);
    }
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::SetBeatFromMouse(QMouseEvent * event)
{
    ED_ASSERT(event != nullptr);

    // Compute the beat selected by the mouse
    float beat = mapToScene(event->pos()).x() / (mHorizontalScale * TIMELINE_BEAT_WIDTH);
    if (beat < 0.0f)
    {
        beat = 0.0f;
    }
    //! \todo Handle out-of-bounds at the end of the timeline

    // Update user interface, including the cursor
    emit BeatUpdated(beat);
}
