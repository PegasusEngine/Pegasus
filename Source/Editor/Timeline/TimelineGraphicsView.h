#ifndef TIMELINEGRAPHICSVIEW_H
#define TIMELINEGRAPHICSVIEW_H

#include <QGraphicsView>


//! Minimum horizontal scale factor
#define TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MIN     0.1f

//! Maximum zoom factor
#define TIMELINE_GRAPHICS_VIEW_HORIZONTAL_SCALE_MAX     10.0f

//! Minimum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MIN                 0.1f

//! Maximum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MAX                 10.0f

//----------------------------------------------------------------------------------------

//! \class TimelineGraphicsView
//! Graphics view rendering the timeline with all its rendering items.
class TimelineGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent widget of the view
    TimelineGraphicsView(QWidget *parent);

    //! Destructor
    ~TimelineGraphicsView();


    //! Create a new lane and add it at the bottom of the list
    void AddLane();


public slots:
//    void shuffle();
//    void zoomIn();
//    void zoomOut();

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
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

    //void keyPressEvent(QKeyEvent *event);
    //void timerEvent(QTimerEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

    //void ScaleView(qreal scaleFactor);

private:

    //! Update the bounds of the scene
    void UpdateSceneRect();


    //! Number of lanes used by the view
    unsigned int mNumLanes;

    //! Horizontal scale of the view
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Zoom level of the view
    //! 1.0f for the original view, < 1.0f for a compressed view, > 1.0f for an expanded view
    float mZoom; 

    //int timerId;
    //Node *centerNode;
};

#endif // TIMELINEGRAPHICSVIEW_H
