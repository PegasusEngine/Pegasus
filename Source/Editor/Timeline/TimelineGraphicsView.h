#ifndef TIMELINEGRAPHICSVIEW_H
#define TIMELINEGRAPHICSVIEW_H

#include <QGraphicsView>


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

    void itemMoved();

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();

protected:
    //void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif
    void drawBackground(QPainter *painter, const QRectF &rect);

    void ScaleView(qreal scaleFactor);

private:
    int timerId;
    //Node *centerNode;
};

#endif // TIMELINEGRAPHICSVIEW_H
