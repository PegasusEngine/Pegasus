/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorGraphicsView.h
//! \author	Kevin Boulanger
//! \date	06th June 2014
//! \brief	Generic graphics view for a graph editor (texture, mesh, etc.)

#include "GraphEditorGraphicsView.h"

//! \todo Temporary
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>


GraphEditorGraphicsView::GraphEditorGraphicsView(QWidget *parent)
:   QGraphicsView(parent)
{
    //! \todo That entire code is temporary.

    // Create the scene containing the items to render
    QGraphicsScene *scene = new QGraphicsScene(this);
    
    // Set the indexing method of the items.
    // NoIndex is supposedly faster when items move a lot.
    // BspTreeIndex is faster for static scenes.
    // Adapt based on the performances.
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // Set the graphics view to the created scene
    setScene(scene);

    QPen pen(Qt::blue);
    QBrush brush(QColor(Qt::blue).lighter(175));
    QGraphicsRectItem * rect = scene->addRect(0.0f, 0.0f, 128.0f, 48.0f, pen, brush);
    rect->setFlag(QGraphicsItem::ItemIsMovable, true);
    QGraphicsSimpleTextItem * text = scene->addSimpleText("Test");
    text->setPos(20.0f, 5.0f);
    text->setParentItem(rect);
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsView::~GraphEditorGraphicsView()
{
}
