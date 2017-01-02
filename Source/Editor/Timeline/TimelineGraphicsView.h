/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineGraphicsView.h
//! \author	Karolyn Boulanger
//! \date	10th June 2013
//! \brief	Graphics view containing the graphics timeline representation

#ifndef EDITOR_TIMELINEGRAPHICSVIEW_H
#define EDITOR_TIMELINEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QList>
#include <QMap>
#include "MessageControllers/MsgDefines.h"

class TimelineBackgroundBeatGraphicsItem;
class TimelineBackgroundBeatLineGraphicsItem;
class TimelineLaneHeaderGraphicsItem;
class TimelineBlockGraphicsItem;
class TimelineCursorGraphicsItem;

class QUndoStack;
class ShadowTimelineState;

//! Minimum horizontal scale factor
#define TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MIN     0.1f

//! Maximum horizontal scale factor
#define TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MAX     10.0f

//! Minimum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MIN                 0.1f

//! Maximum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MAX                 10.0f

//----------------------------------------------------------------------------------------

//! Graphics view rendering the timeline with all its rendering items.
class TimelineGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent widget of the view
    TimelineGraphicsView(QWidget *parent);

    //! Destructor
    virtual ~TimelineGraphicsView();

    //! Sets the undo stack for this widget element
    void SetUndoStack(QUndoStack* undoStack) { mUndoStack = undoStack; } 

    //! Gets the undo stack for this widget element
    QUndoStack* GetUndoStack() const { return mUndoStack; }

    //! Set the antialiasing mode of the timeline
    //! \param enable True to enable antialiasing
    void EnableAntialiasing(bool enable);

    //! Clear the entire content of the graphics scene and initialize it to a default timeline
    void Initialize();

    //! Refresh the content of the graphics view from the application timeline
    void RefreshFromTimeline();

    //! Sets the current timeline.
    void SetTimeline(ShadowTimelineState* timeline ) { mTimeline = timeline;}

    //! Flushes (copies) all internal visual properties to intermediate buffers
    void FlushVisualProperties();

    //! Forces a redraw on all internal graphics items.
    void RedrawInternalBlocks();

    //! Updates the visuals of a beat.
    void UpdateBeatVisuals(int newLane, unsigned blockGuid, unsigned newBeat);

    //! Finds a lane id for a particular block guid
    //! returns -1 if the lane does not exist. Fills in the block pointer if found in lane.
    int FindLane(unsigned blockGuid, TimelineBlockGraphicsItem*& outBlock) const;

    //! Gets the current beat that the cursor is pointing to.
    float GetBeatFromCursor() const;

    //------------------------------------------------------------------------------------

signals:

    //! Sent when the current beat has been updated by user interaction in the view
    //! \param beat Current beat, can have fractional part
    void BeatUpdated(float beat);

    //! Emitted when a single block is being selected (not multiple selection)
    void BlockSelected(unsigned blockGuid);

    //! Emitted when multiple blocks are being selected (not single selection)
    void MultiBlocksSelected();

    //! Emitted when blocks are deselected (single or multiple selection)
    void BlocksDeselected();

    //! Emitted when the user double clicks a block
    void BlockDoubleClicked(QString blockscriptToOpen);

    //! Request this script to be changed for the block with the guid.
    void RequestChangeScript(QGraphicsObject* sender, unsigned blockId);

    //! Request the blockscript to be removed
    void RequestRemoveScript(QGraphicsObject* sender, unsigned blockId);
    
    //! Request the blockscript to be removed
    void RequestBlockMove(QGraphicsObject* sender, QPointF amount);

    //! Requests draw all viewports
    void RequestDrawAllViewports();

public slots:

    //! Create a new lane and add it at the bottom of the list
    void AddLane();

    //! Set the position of the cursor from the given beat
    //! \param beat Current beat, can have fractional part
    void SetCursorFromBeat(float beat);

    //! Called when play mode is toggled
    //! \param enable True when play mode is enabled, resulting in disabling the update of the beat
    //!               on the timeline when right-click dragging
    void OnPlayModeToggled(bool enable);

    //! Focus selection on the block with the specified guid.
    //! \param the guid of the block to focus onto.
    void OnFocusBlock(unsigned blockGuid);

    //! Action triggered when the horizontal bar moves.
    void OnHorizontalScroll(int amount);


    //! Refresh the content of a lane using the data from the application timeline lane
    //! \param laneIndex Index of the lane to refresh (< mNumLanes)
    //! \param laneProxy Proxy of the timeline lane to get the data from
    void RefreshLaneFromTimelineLane(unsigned int laneIndex, const ShadowLaneState& lane);

    //------------------------------------------------------------------------------------

private slots:

    //! Called when the selection changes in the scene
    void SelectionChanged();
        
    //------------------------------------------------------------------------------------

protected:

    //! Set the horizontal scale of the view
    //! \param scale 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    void SetHorizontalScale(float scale);

    //! Multiply the horizontal scale of the view, limiting it to the valid range
    //! \param scaleFactor Factor centered at 1.0f, multiplying the horizontal factor (> 0.0f)
    void MultiplyHorizontalScale(float scaleFactor);

    //! Set the zoom level of the view
    //! \param zoom 1.0f for the original view, < 1.0f for a compressed view, > 1.0f for an expanded view
    void SetZoom(float zoom);

    //! Multiply the zoom level of the view, limiting it to the valid range
    //! \param zoomFactor Factor centered at 1.0f, multiplying the zoom level (> 0.0f)
    void MultiplyZoom(float zoomFactor);


    // QGraphicsView overloads

    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

    //------------------------------------------------------------------------------------

private:

    //! Clear the entire content of the graphics scene
    void Clear();

    //! Update the bounds of the scene
    void UpdateSceneRect();

    //! Create new lanes, without affecting the application
    //! \param firstLane Index of the first lane (<= mNumLanes)
    //! \param numLanes Number of lanes to add (>= 1)
    void CreateLanes(unsigned int firstLane, unsigned int numLanes);

    //! Remove existing lanes, without affecting the application
    //! \param firstLane Index of the first lane (<= mNumLanes)
    //! \param numLanes Number of lanes to remove (>= 1)
    void RemoveLanes(unsigned int firstLane, unsigned int numLanes);

    //! Create new background graphics items, typically used when extending the length of the timeline
    //! \param firstBeat Index of the first beat (<= mNumBeats)
    //! \param numBeats Number of beats to add (>= 1)
    void CreateBackgroundGraphicsItems(unsigned int firstBeat, unsigned int numBeats);

    //! Remove existing background graphics items, typically used when reducing the length of the timeline
    //! \param firstBeat Index of the first beat (<= mNumBeats)
    //! \param numBeats Number of beats to remove (>= 1)
    void RemoveBackgroundGraphicsItems(unsigned int firstBeat, unsigned int numBeats);

    //! Remove existing block graphics items for a lane
    //! \param laneIndex Index of the lane to clear
    void ClearBlockItems(unsigned int laneIndex);

    //! Sets the lane header positions aligned to the current view of the horizontal scrollbar.
    void ResetLaneHeaderPositions();

    //! Called when a right-click or right-dragging occurs, this then sets the current beat and updates the UI
    //! \param event Qt mouse event
    void SetBeatFromMouse(QMouseEvent * event);

    //! True to update the beat on the timeline when right-click dragging (true by default)
    bool mRightClickCursorDraggingEnabled;

    //! Number of beats defining the length of the timeline (>= 1)
    unsigned int mNumBeats;

    //! Number of lanes used by the view
    unsigned int mNumLanes;

    //! Horizontal scale of the view
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Zoom level of the view
    //! 1.0f for the original view, < 1.0f for a compressed view, > 1.0f for an expanded view
    float mZoom; 

    //! List of background beat graphics items (filled rectangles)
    QList<TimelineBackgroundBeatGraphicsItem *> mBackgroundBeatItems;

    //! List of background beat line graphics items (vertical lines and beat numbers)
    QList<TimelineBackgroundBeatLineGraphicsItem *> mBackgroundBeatLineItems;

    //! List of lane header graphics items (block with name)
    QList<TimelineLaneHeaderGraphicsItem *> mLaneHeaderItems;

    //! Type for a list of block guids stored in a timeline lane
    typedef QMap<unsigned int, TimelineBlockGraphicsItem *> LaneBlockList;

    //! List of blocks for each lane of the timeline
    QList<LaneBlockList> mBlockItems;

    //! List of block graphics items
    //TimelineBlockGraphicsItem
    //! \todo Handle lists of blocks

    //! Cursor graphics items
    TimelineCursorGraphicsItem * mCursorItem;

    //! Undo stack
    QUndoStack * mUndoStack;

    //! The current timeline
    ShadowTimelineState* mTimeline;
};


#endif  // EDITOR_TIMELINEGRAPHICSVIEW_H
