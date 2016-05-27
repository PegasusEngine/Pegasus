/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorGraphicsView.h
//! \author	Karolyn Boulanger
//! \date	06th June 2014
//! \brief	Generic graphics view for a graph editor (texture, mesh, etc.)

#include "Graph/GraphEditorGraphicsView.h"
#include "Graph/Items/GraphNodeGraphicsItem.h"
#include "Graph/Items/GraphConnectionGraphicsItem.h"

//! \todo Temporary
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>


GraphEditorGraphicsView::GraphEditorGraphicsView(QWidget *parent)
:   QGraphicsView(parent)
{
    // Set the background to a tilable grid image
    //! \todo Use a common picture repository
    QPixmap backgroundPix(":/GraphEditor/GraphBackground.png");
    QBrush backgroundBrush(backgroundPix);
    setBackgroundBrush(backgroundBrush);
    
    // Enable antialiasing. It looks really bad otherwise
    setRenderHint(QPainter::Antialiasing, true);

    // Create the scene containing the items to render
    QGraphicsScene *scene = new QGraphicsScene(this);
    
    // Set the indexing method of the items.
    // NoIndex is supposedly faster when items move a lot.
    // BspTreeIndex is faster for static scenes.
    // Adapt based on the performances.
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // Set the graphics view to the created scene
    setScene(scene);
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsView::~GraphEditorGraphicsView()
{
}

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem* GraphEditorGraphicsView::CreateNode(const QString& title, const QList<QString>& inputList)
{
    // Create the node item and add it to the scene
    //! \todo Add the undo stack
    GraphNodeGraphicsItem* nodeItem = new GraphNodeGraphicsItem(title, scene(), /***/nullptr);
    scene()->addItem(nodeItem);

    // Add all declared inputs
    foreach (const QString& inputName, inputList)
    {
        nodeItem->AddInput(inputName);
    }

    /*******/
    //! \todo TEMPORARY position calculation
    static unsigned int sPosIndex = 0;
         if (sPosIndex == 0) nodeItem->setPos(0.0f, 0.0f);
    else if (sPosIndex == 1) nodeItem->setPos(0.0f, 64.0f);
    else if (sPosIndex == 2) nodeItem->setPos(1.5f * 128.0f, 0.0f);
    else if (sPosIndex == 3) nodeItem->setPos(3.0f * 128.0f, 0.0f);
    sPosIndex++;
    /*******/

    return nodeItem;
}

//----------------------------------------------------------------------------------------

GraphConnectionGraphicsItem* GraphEditorGraphicsView::CreateConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                                                       GraphNodeInputGraphicsItem* dstInput)
{
    if ((srcOutput != nullptr) && (dstInput != nullptr))
    {
        // Create the connection item and add it to the scene
        //! \todo Add the undo stack
        GraphConnectionGraphicsItem* connectionItem = new GraphConnectionGraphicsItem(srcOutput, dstInput, scene(), /***/nullptr);
        scene()->addItem(connectionItem);

        return connectionItem;
    }
    else
    {
        if (srcOutput == nullptr)
        {
            ED_FAILSTR("Trying to create a connection between two nodes, but the source node output is null");
        }
        else
        {
            ED_FAILSTR("Trying to create a connection between two nodes, but the destination node input is null");
        }
    }

    return nullptr;
}
