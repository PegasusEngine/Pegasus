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
#include "Graph/GraphEditorGraphicsScene.h"


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
    mScene = new GraphEditorGraphicsScene(this);
    
    // Set the indexing method of the items.
    // NoIndex is supposedly faster when items move a lot.
    // BspTreeIndex is faster for static scenes.
    // Adapt based on the performances.
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // Set the graphics view to the created scene
    setScene(mScene);

    //! Enable user interactions with the scene (mouse events)
    setInteractive(true);
    
    //! Enable the rubber band selection when clicking in an empty space of the scene
    setDragMode(QGraphicsView::RubberBandDrag);

    //! \todo Why is it not working? false means that mouseMoveEvent should be called only when a mouse button is pressed
    setMouseTracking(false);
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsView::~GraphEditorGraphicsView()
{
}
