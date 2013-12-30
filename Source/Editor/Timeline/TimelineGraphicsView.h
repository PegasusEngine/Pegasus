/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineGraphicsView.h
//! \author	Kevin Boulanger
//! \date	10th June 2013
//! \brief	Graphics view containing the graphics timeline representation

#ifndef EDITOR_TIMELINEGRAPHICSVIEW_H
#define EDITOR_TIMELINEGRAPHICSVIEW_H

#include <QGraphicsView>

class TimelineCursorGraphicsItem;


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


    //! Set the antialiasing mode of the timeline
    //! \param enable True to enable antialiasing
    void EnableAntialiasing(bool enable);


signals:

    //! Sent when the current beat has been updated by user interaction in the view
    //! \param beat Current beat, can have fractional part
    void BeatUpdated(float beat);


public slots:

    //! Create a new lane and add it at the bottom of the list
    void AddLane();

    //! Set the position of the cursor from the given beat
    //! \param beat Current beat, can have fractional part
    void SetCursorFromBeat(float beat);

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

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

    //void drawBackground(QPainter *painter, const QRectF &rect);
    //void keyPressEvent(QKeyEvent *event);
    //void timerEvent(QTimerEvent *event);

private:

    //! Update the bounds of the scene
    void UpdateSceneRect();

    //! Called when a right-click or right-dragging occurs, this then sets the current beat and updates the UI
    //! \param event Qt mouse event
    void SetBeatFromMouse(QMouseEvent * event);

    //! Number of lanes used by the view
    unsigned int mNumLanes;

    //! Horizontal scale of the view
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Zoom level of the view
    //! 1.0f for the original view, < 1.0f for a compressed view, > 1.0f for an expanded view
    float mZoom; 

    //! Cursor graphics items
    TimelineCursorGraphicsItem * mCursorItem;
};


#endif  // EDITOR_TIMELINEGRAPHICSVIEW_H
