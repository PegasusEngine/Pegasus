/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineGraphicsView.cpp
//! \author	Kevin Boulanger
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

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"

#include <QWheelEvent>


TimelineGraphicsView::TimelineGraphicsView(QWidget *parent)
:   QGraphicsView(parent),
    mRightClickCursorDraggingEnabled(true),
    mNumBeats(32),
    mNumLanes(0),
    mHorizontalScale(1.0f),
    mZoom(1.0f)
{
    // Set the scrollbars to be always visible
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Create the scene containing the items to render
    QGraphicsScene *scene = new QGraphicsScene(this);
    
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

    // Create the cursor
    mCursorItem = new TimelineCursorGraphicsItem(1, mHorizontalScale);
    scene->addItem(mCursorItem);

    // Create the default lane
    CreateLanes(0, 1);
    CreateBackgroundGraphicsItems(0, mNumBeats);

    //! \todo Handle timeline blocks
    //TimelineBlockGraphicsItem * item1 = new TimelineBlockGraphicsItem(0, 0.0f, 1.0f, QColor(255, 128, 128), mHorizontalScale);
    //TimelineBlockGraphicsItem * item2 = new TimelineBlockGraphicsItem(1, 2.0f, 4.0f, QColor(128, 255, 128), mHorizontalScale);
    //TimelineBlockGraphicsItem * item3 = new TimelineBlockGraphicsItem(3, 0.5f, 2.0f, QColor(128, 128, 255), mHorizontalScale);
    //scene->addItem(item1);
    //scene->addItem(item2);
    //scene->addItem(item3);
}

//----------------------------------------------------------------------------------------

TimelineGraphicsView::~TimelineGraphicsView()
{
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

        Pegasus::Timeline::ITimelineProxy * const timeline = application->GetTimelineProxy();

        // Handle the length of the timeline
        const unsigned int numBeats = timeline->GetNumBeats();
        if (numBeats < mNumBeats)
        {
            // Remove the background items at the end of the timeline
            //! \todo Implement
            /*****/
            mNumBeats = numBeats;
        }
        else if (numBeats > mNumBeats)
        {
            // Add the missing background items at the end of the timeline
            CreateBackgroundGraphicsItems(mNumBeats, numBeats - mNumBeats);
            mNumBeats = numBeats;
        }

        // Handle the number of lanes of the timeline
        const unsigned int numLanes = timeline->GetNumLanes();
        if (numLanes < mNumLanes)
        {
            // Remove the extra lanes from the end of the list
            //! \todo Implement
            /*****/
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
            //RefreshLaneFromTimelineLane(l, timeline->GetLane(l));
        }
    }
    else
    {
        // Application closed or failed to load

        // Reset the timeline graphics view to its default state
        //! \todo Implement
        /****/
    }
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

    // Reposition the cursor
    if (mCursorItem != nullptr)
    {
        mCursorItem->SetBeat(beat);
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

void TimelineGraphicsView::SetHorizontalScale(float scale)
{
    ED_ASSERTSTR((scale >= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MIN) && (scale <= TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MAX),
                 "Invalid horizontal scale (%f) for the timeline graphics view.", scale);
    mHorizontalScale = scale;

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Update the horizontal scale of all the blocks
    // (this invalidates the cache of the block graphics items)
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
    //! \todo List of blocks
    //TimelineBlockGraphicsItem * blockItem = qgraphicsitem_cast<TimelineBlockGraphicsItem *>(item);
    //if (blockItem != nullptr)
    //{
    //    blockItem->SetHorizontalScale(mHorizontalScale);
    //    continue;
    //}
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
    ED_ASSERTSTR(numLanes >= 1, "Invalid number of lanes (%d), it should be >= 1", numLanes);
    const unsigned int lastLane = firstLane + numLanes;

    // Invalidate the cache of the view, so that the background does not keep
    // ghosts of the previous blocks
    resetCachedContent();

    // Create the lane specific graphics items
    unsigned int l;
    for (l = firstLane; l < lastLane; ++l)
    {
        // Add a lane header
        //! \todo Add list
        TimelineLaneHeaderGraphicsItem * laneHeaderItem = new TimelineLaneHeaderGraphicsItem(l);
        scene()->addItem(laneHeaderItem);
    }
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

void TimelineGraphicsView::CreateBackgroundGraphicsItems(unsigned int firstBeat, unsigned int numBeats)
{
    ED_ASSERTSTR(firstBeat <= mNumBeats, "Invalid first beat (%d), it should be <= %d", firstBeat, mNumBeats);
    ED_ASSERTSTR(numBeats >= 1, "Invalid number of beats (%d), it should be >= 1", numBeats);
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

void TimelineGraphicsView::RefreshLaneFromTimelineLane(unsigned int laneIndex, Pegasus::Timeline::ILaneProxy * laneProxy)
{
    /****/
    //! \todo Implement
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

//----------------------------------------------------------------------------------------

//void GraphWidget::keyPressEvent(QKeyEvent *event)
//{
//    switch (event->key()) {
//    case Qt::Key_Up:
//        centerNode->moveBy(0, -20);
//        break;
//    case Qt::Key_Down:
//        centerNode->moveBy(0, 20);
//        break;
//    case Qt::Key_Left:
//        centerNode->moveBy(-20, 0);
//        break;
//    case Qt::Key_Right:
//        centerNode->moveBy(20, 0);
//        break;
//    case Qt::Key_Plus:
//        zoomIn();
//        break;
//    case Qt::Key_Minus:
//        zoomOut();
//        break;
//    case Qt::Key_Space:
//    case Qt::Key_Enter:
//        shuffle();
//        break;
//    default:
//        QGraphicsView::keyPressEvent(event);
//    }
//}