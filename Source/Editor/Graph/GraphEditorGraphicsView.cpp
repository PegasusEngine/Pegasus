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

    QPen pen/*(Qt::NoPen)*/;
    QLinearGradient generatorGradient(0.0f, 0.0f, 0.0f, 32.0f);
    generatorGradient.setColorAt(0, QColor(255, 0, 0, 255).lighter(160));
    generatorGradient.setColorAt(1, QColor(255, 0, 0, 255).lighter(160).darker(120));
    QBrush generatorBrush(generatorGradient);
    QLinearGradient operatorGradient(0.0f, 32.0f, 0.0f, 64.0f);
    operatorGradient.setColorAt(0, QColor(100, 230, 230, 255));
    operatorGradient.setColorAt(1, QColor(100, 230, 230, 255).darker(120));
    QBrush operatorBrush(operatorGradient);
    QLinearGradient assetGradient(0.0f, 64.0f, 0.0f, 96.0f);
    assetGradient.setColorAt(0, QColor(192, 192, 192, 255));
    assetGradient.setColorAt(1, QColor(192, 192, 192, 255).darker(120));
    QBrush assetBrush(assetGradient);

    QGraphicsRectItem * rect = scene->addRect(-128.0f, 0.0f, 128.0f, 32.0f, pen, generatorBrush);
    rect->setFlag(QGraphicsItem::ItemIsMovable, true);
    QGraphicsSimpleTextItem * text = scene->addSimpleText("Gradient");
    text->setPos(-88.0f, 8.0f);
    text->setParentItem(rect);

    QGraphicsRectItem * rect2 = scene->addRect(0.0f, 0.0f, 128.0f, 32.0f, pen, generatorBrush);
    rect2->setFlag(QGraphicsItem::ItemIsMovable, true);
    QGraphicsSimpleTextItem * text2 = scene->addSimpleText("Color");
    text2->setPos(48.0f, 8.0f);
    text2->setParentItem(rect2);

    QGraphicsRectItem * rect3 = scene->addRect(-128.0f, 32.0f, 256.0f, 32.0f, pen, operatorBrush);
    rect3->setFlag(QGraphicsItem::ItemIsMovable, true);
    QGraphicsSimpleTextItem * text3 = scene->addSimpleText("Add");
    text3->setPos(-16.0f, 40.0f);
    text3->setParentItem(rect3);

    QGraphicsRectItem * rect4 = scene->addRect(-128.0f, 64.0f, 128.0f, 32.0f, pen, assetBrush);
    rect4->setFlag(QGraphicsItem::ItemIsMovable, true);
    QGraphicsSimpleTextItem * text4 = scene->addSimpleText("<Gradient1>");
    text4->setPos(-104.0f, 72.0f);
    text4->setParentItem(rect4);
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsView::~GraphEditorGraphicsView()
{
}
