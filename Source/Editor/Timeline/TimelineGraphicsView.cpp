#include "TimelineGraphicsView.h"
#include "TimelineBlockGraphicsItem.h"

#include <QWheelEvent>


//! Zoom factor applied when using the mouse wheel (> 1.0f)
#define TIMELINE_GRAPHICS_VIEW_ZOOM_INCREMENT       1.2f

//! Minimum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MINIMUM         0.01f

//! Maximum zoom factor
#define TIMELINE_GRAPHICS_VIEW_ZOOM_MAXIMUM         10.0f

//----------------------------------------------------------------------------------------

TimelineGraphicsView::TimelineGraphicsView(QWidget *parent)
:   QGraphicsView(parent), timerId(0)
{
    // Create the scene containing the items to render
    QGraphicsScene *scene = new QGraphicsScene(this);
    
    // Set the indexing method of the items.
    // NoIndex is supposedly faster when items move a lot.
    // BspTreeIndex is faster for static scenes.
    // Adapt based on the performances.
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // Set the initial bounds of the scene
    //! \todo Use variables
    scene->setSceneRect(/***/-64.0f, -16.0f, 32.0f * 128.0f + 64.0f, 10.0f * 32.0f + 16.0f);
    setScene(scene);

    // Set the cache mode
    //! \todo Update based on performances
    setCacheMode(CacheBackground);

    // Set the update mode, when updating the content of the view
    //! \todo Change based on the performances
    setViewportUpdateMode(BoundingRectViewportUpdate);

    // Enable anti-aliasing
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);

    // Use the mouse position to focus the zoom
    setTransformationAnchor(AnchorUnderMouse);

    // Initial scale of the rendering
    //! \todo Use variables
    scale(1.0f, 1.0f);

    //! \todo **** Temporary block to create items to render
    TimelineBlockGraphicsItem * item1 = new TimelineBlockGraphicsItem(0.0f, 1.0f);
    TimelineBlockGraphicsItem * item2 = new TimelineBlockGraphicsItem(2.0f, 4.0f);
    TimelineBlockGraphicsItem * item3 = new TimelineBlockGraphicsItem(0.5f, 2.0f);
    scene->addItem(item1);
    scene->addItem(item2);
    scene->addItem(item3);
}

//----------------------------------------------------------------------------------------

TimelineGraphicsView::~TimelineGraphicsView()
{
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
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

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    //QList<Node *> nodes;
    //foreach (QGraphicsItem *item, scene()->items()) {
    //    if (Node *node = qgraphicsitem_cast<Node *>(item))
    //        nodes << node;
    //}

    //foreach (Node *node, nodes)
    //    node->calculateForces();

    bool itemsMoved = false;
    //foreach (Node *node, nodes) {
    //    if (node->advance())
    //        itemsMoved = true;
    //}

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

//----------------------------------------------------------------------------------------

#ifndef QT_NO_WHEELEVENT
void TimelineGraphicsView::wheelEvent(QWheelEvent *event)
{
    ScaleView(pow((double)2, event->delta() / 240.0));
}
#endif

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::ScaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, /*scaleFactor*/1.0f).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < TIMELINE_GRAPHICS_VIEW_ZOOM_MINIMUM || factor > TIMELINE_GRAPHICS_VIEW_ZOOM_MAXIMUM)
        return;

    scale(scaleFactor, /*scaleFactor*/1.0f);
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::shuffle()
{
    //foreach (QGraphicsItem *item, scene()->items()) {
    //    if (qgraphicsitem_cast<Node *>(item))
    //        item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    //}
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::zoomIn()
{
    ScaleView(qreal(1.2));
}

//----------------------------------------------------------------------------------------

void TimelineGraphicsView::zoomOut()
{
    ScaleView(1 / qreal(1.2));
}
