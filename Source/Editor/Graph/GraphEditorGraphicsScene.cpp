/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorGraphicsScene.cpp
//! \author	Karolyn Boulanger
//! \date	14th June 2016
//! \brief	Generic graphics scene for a graph editor (texture, mesh, etc.)

#include "Graph/GraphEditorGraphicsScene.h"
#include "Graph/Items/GraphNodeGraphicsItem.h"
#include "Graph/Items/GraphNodeInputGraphicsItem.h"
#include "Graph/Items/GraphNodeOutputGraphicsItem.h"
#include "Graph/Items/GraphConnectionGraphicsItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>


GraphEditorGraphicsScene::GraphEditorGraphicsScene(QObject* parent)
:   QGraphicsScene(parent)
,   mFloatingDstConnectionItem(nullptr)
,   mFloatingSrcConnectionItem(nullptr)
{
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsScene::~GraphEditorGraphicsScene()
{
    // Delete all the connections first, otherwise we will have dangling pointers inside the nodes
    // during destruction
    foreach(QGraphicsItem* item, items())
    {
        GraphConnectionGraphicsItem* connectionItem = qgraphicsitem_cast<GraphConnectionGraphicsItem*>(item);
        if (connectionItem != nullptr)
        {
            RemoveConnection(connectionItem);
        }
    }

    // Clear the nodes of the scene (manually done here, but the parent destructor does it anyway)
    clear();
}

//----------------------------------------------------------------------------------------

GraphEditorGraphicsScene::SceneMode GraphEditorGraphicsScene::GetSceneMode() const
{
    if (mFloatingDstConnectionItem != nullptr)
    {
        ED_ASSERT(mFloatingSrcConnectionItem == nullptr);
        return SCENEMODE_FLOATING_DST_CONNECTION;
    }
    else if (mFloatingSrcConnectionItem != nullptr)
    {
        ED_ASSERT(mFloatingDstConnectionItem == nullptr);
        return SCENEMODE_FLOATING_SRC_CONNECTION;
    }
    else
    {
        ED_ASSERT(mFloatingDstConnectionItem == nullptr);
        ED_ASSERT(mFloatingSrcConnectionItem == nullptr);
        return SCENEMODE_IDLE;
    }
}

//----------------------------------------------------------------------------------------

GraphNodeGraphicsItem* GraphEditorGraphicsScene::CreateNode(const QString& title, const QList<QString>& inputList)
{
    // Create the node item and add it to the scene
    //! \todo Add the undo stack
    GraphNodeGraphicsItem* nodeItem = new GraphNodeGraphicsItem(title, this, /***/nullptr);
    addItem(nodeItem);

    // Add all declared inputs
    foreach (const QString& inputName, inputList)
    {
        nodeItem->AddInput(inputName);
    }

    connect(nodeItem->GetOutputItem(), SIGNAL(StartFloatingDstConnection(GraphNodeOutputGraphicsItem*, QGraphicsSceneMouseEvent*)),
            this, SLOT(StartFloatingDstConnection(GraphNodeOutputGraphicsItem*, QGraphicsSceneMouseEvent*)));
    connect(nodeItem->GetOutputItem(), SIGNAL(EndFloatingDstConnection(GraphNodeOutputGraphicsItem*, QGraphicsSceneMouseEvent*)),
            this, SLOT(EndFloatingDstConnection(GraphNodeOutputGraphicsItem*, QGraphicsSceneMouseEvent*)));

    for (unsigned int input = 0; input < nodeItem->GetNumInputs(); ++input)
    {
        connect(nodeItem->GetInputItem(input), SIGNAL(StartFloatingSrcConnection(GraphNodeInputGraphicsItem*, QGraphicsSceneMouseEvent*)),
                this, SLOT(StartFloatingSrcConnection(GraphNodeInputGraphicsItem*, QGraphicsSceneMouseEvent*)));
        connect(nodeItem->GetInputItem(input), SIGNAL(EndFloatingSrcConnection(GraphNodeInputGraphicsItem*, QGraphicsSceneMouseEvent*)),
                this, SLOT(EndFloatingSrcConnection(GraphNodeInputGraphicsItem*, QGraphicsSceneMouseEvent*)));
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

void GraphEditorGraphicsScene::RemoveNode(GraphNodeGraphicsItem* nodeItem)
{
    if (nodeItem != nullptr)
    {
        // Delete the input connections if defined
        const unsigned int numInputs = nodeItem->GetNumInputs();
        for (unsigned int input = 0; input < numInputs; ++input)
        {
            GraphNodeInputGraphicsItem* inputItem = nodeItem->GetInputItem(input);
            ED_ASSERT(inputItem != nullptr);
            GraphConnectionGraphicsItem* inputConnection = inputItem->GetConnection();
            if (inputConnection != nullptr)
            {
                RemoveConnection(inputConnection);
            }
        }

        // Delete the output connections if defined
        GraphNodeOutputGraphicsItem* outputItem = nodeItem->GetOutputItem();
        if (outputItem != nullptr)
        {
            const unsigned int numConnections = outputItem->GetNumConnections();
            for (unsigned int connection = 0; connection < numConnections; ++connection)
            {
                GraphConnectionGraphicsItem* outputConnection = outputItem->GetConnection(connection);
                if (outputConnection != nullptr)
                {
                    RemoveConnection(outputConnection);
                }
            }
        }

        // Delete the node itself
        delete nodeItem;
    }
    else
    {
        ED_FAILSTR("Trying to remove an invalid node.");
    }
}

//----------------------------------------------------------------------------------------

GraphConnectionGraphicsItem* GraphEditorGraphicsScene::CreateConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                                                        GraphNodeInputGraphicsItem* dstInput)
{
    if ((srcOutput != nullptr) && (dstInput != nullptr))
    {
        // Create the connection item and add it to the scene
        //! \todo Add the undo stack
        GraphConnectionGraphicsItem* connectionItem = new GraphConnectionGraphicsItem(srcOutput, dstInput, this, /***/nullptr);
        addItem(connectionItem);

        // Update the output and input to know we are connected to them
        srcOutput->AddConnection(connectionItem);
        dstInput->SetConnection(connectionItem);

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

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::RemoveConnection(GraphConnectionGraphicsItem* connectionItem)
{
    if (connectionItem != nullptr)
    {
        if (!connectionItem->IsFloating())
        {
            connectionItem->GetSrcOutput()->DisconnectConnection(connectionItem);
            connectionItem->GetDstInput()->DisconnectConnection();
        }

        delete connectionItem;
    }
    else
    {
        ED_FAILSTR("Trying to remove an invalid connection.");
    }
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::StartFloatingDstConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                                          QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mFloatingDstConnectionItem != nullptr)
    {
        // If the user clicks too fast, a new press event might happen before the release event occurs.
        // In that case, forcefully process the previous floating connection, then take care of the new one.
        EndFloatingDstConnection(srcOutput, mouseEvent);
    }
    
    // Create the floating connection item and add it to the scene
    //! \todo Add the undo stack
    GraphConnectionGraphicsItem* connectionItem = new GraphConnectionGraphicsItem(srcOutput, nullptr, this, /***/nullptr);
    addItem(connectionItem);

    // Store the current floating connection, so the mouse movements affect it
    mFloatingDstConnectionItem = connectionItem;

    // Initial update of the connection end point
    mFloatingDstConnectionItem->AdjustDstGeometry(mouseGrabberItem()->scenePos());
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::EndFloatingDstConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                                        QGraphicsSceneMouseEvent* mouseEvent)
{
    if (GetSceneMode() == SCENEMODE_FLOATING_DST_CONNECTION)
    {
        GraphNodeInputGraphicsItem* const inputItem = GetInputItemAtScenePos(mouseEvent->scenePos());

        // Delete the floating connection
        RemoveConnection(mFloatingDstConnectionItem);
        mFloatingDstConnectionItem = nullptr;

        // If an input node item is under the mouse cursor, create the new connection
        if (inputItem != nullptr)
        {
            //! \todo Test for the validity of the connection?

            // If the input has already a connection, remove the previous connection
            GraphConnectionGraphicsItem* previousConnectionItem = inputItem->GetConnection();
            if (previousConnectionItem != nullptr)
            {
                RemoveConnection(previousConnectionItem);
            }
            
            CreateConnection(srcOutput, inputItem);
        }
    }
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::StartFloatingSrcConnection(GraphNodeInputGraphicsItem* dstInput,
                                                          QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mFloatingSrcConnectionItem != nullptr)
    {
        // If the user clicks too fast, a new press event might happen before the release event occurs.
        // In that case, forcefully process the previous floating connection, then take care of the new one.
        EndFloatingSrcConnection(dstInput, mouseEvent);
    }

    // Create the floating connection item and add it to the scene
    //! \todo Add the undo stack
    GraphConnectionGraphicsItem* connectionItem = new GraphConnectionGraphicsItem(nullptr, dstInput, this, /***/nullptr);
    addItem(connectionItem);

    // Store the current floating connection, so the mouse movements affect it
    mFloatingSrcConnectionItem = connectionItem;

    // Initial update of the connection end point
    mFloatingSrcConnectionItem->AdjustSrcGeometry(mouseGrabberItem()->scenePos());
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::EndFloatingSrcConnection(GraphNodeInputGraphicsItem* dstInput,
                                                        QGraphicsSceneMouseEvent* mouseEvent)
{
    if (GetSceneMode() == SCENEMODE_FLOATING_SRC_CONNECTION)
    {
        GraphNodeOutputGraphicsItem* const outputItem = GetOutputItemAtScenePos(mouseEvent->scenePos());

        // Delete the floating connection
        RemoveConnection(mFloatingSrcConnectionItem);
        mFloatingSrcConnectionItem = nullptr;

        // If an output node item is under the mouse cursor, create the new connection
        if (outputItem != nullptr)
        {
            //! \todo Test for the validity of the connection?

            // If the input has already a connection, remove the previous connection
            GraphConnectionGraphicsItem* previousConnectionItem = dstInput->GetConnection();
            if (previousConnectionItem != nullptr)
            {
                RemoveConnection(previousConnectionItem);
            }
            
            CreateConnection(outputItem, dstInput);
        }
    }
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->matches(QKeySequence::Delete))
    {
        DeleteSelectedItems();
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (GetSceneMode() == SCENEMODE_FLOATING_DST_CONNECTION)
    {
        const GraphNodeInputGraphicsItem* const inputItem = GetInputItemAtScenePos(mouseEvent->scenePos());
        if (inputItem != nullptr)
        {
            //! \todo Test for the validity of the connection

            // If an input item is under the mouse cursor, snap the floating connection to it
            mFloatingDstConnectionItem->AdjustDstGeometry(inputItem->scenePos());
        }
        else
        {
            // If no input item is under the mouse cursor, have the floating connection follow the mouse
            mFloatingDstConnectionItem->AdjustDstGeometry(mouseEvent->scenePos());
        }
    }
    else if (GetSceneMode() == SCENEMODE_FLOATING_SRC_CONNECTION)
    {
        const GraphNodeOutputGraphicsItem* const outputItem = GetOutputItemAtScenePos(mouseEvent->scenePos());
        if (outputItem != nullptr)
        {
            //! \todo Test for the validity of the connection

            // If an output item is under the mouse cursor, snap the floating connection to it
            mFloatingSrcConnectionItem->AdjustSrcGeometry(outputItem->scenePos());
        }
        else
        {
            // If no output item is under the mouse cursor, have the floating connection follow the mouse
            mFloatingSrcConnectionItem->AdjustSrcGeometry(mouseEvent->scenePos());
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

//----------------------------------------------------------------------------------------

GraphNodeInputGraphicsItem* GraphEditorGraphicsScene::GetInputItemAtScenePos(const QPointF& mouseScenePos) const
{
    return qgraphicsitem_cast<GraphNodeInputGraphicsItem*>(itemAt(mouseScenePos, QTransform()));
}

//----------------------------------------------------------------------------------------

GraphNodeOutputGraphicsItem* GraphEditorGraphicsScene::GetOutputItemAtScenePos(const QPointF& mouseScenePos) const
{
    return qgraphicsitem_cast<GraphNodeOutputGraphicsItem*>(itemAt(mouseScenePos, QTransform()));
}

//----------------------------------------------------------------------------------------

void GraphEditorGraphicsScene::DeleteSelectedItems()
{
    QList<QGraphicsItem *> itemsToDelete = selectedItems();

    // Delete the connections first
    foreach(QGraphicsItem* item, itemsToDelete)
    {
        GraphConnectionGraphicsItem* connectionItem = qgraphicsitem_cast<GraphConnectionGraphicsItem*>(item);
        if (connectionItem != nullptr)
        {
            RemoveConnection(connectionItem);
            itemsToDelete.removeOne(item);
        }
    }

    // Delete the node input and node output items.
    // If we do not do it now, we could get node input or output items left in the list
    // after nodes are deleted, resulting in dangling pointer deletion.
    foreach(QGraphicsItem* item, itemsToDelete)
    {
        GraphNodeInputGraphicsItem* inputItem = qgraphicsitem_cast<GraphNodeInputGraphicsItem*>(item);
        if (inputItem != nullptr)
        {
            itemsToDelete.removeOne(item);
        }
        else
        {
            GraphNodeOutputGraphicsItem* outputItem = qgraphicsitem_cast<GraphNodeOutputGraphicsItem*>(item);
            if (outputItem != nullptr)
            {
                itemsToDelete.removeOne(item);
            }
        }
    }

    // Delete the nodes (the inputs and outputs will be deleted automatically)
    foreach(QGraphicsItem* item, itemsToDelete)
    {
        GraphNodeGraphicsItem* nodeItem = qgraphicsitem_cast<GraphNodeGraphicsItem*>(item);
        if (nodeItem != nullptr)
        {
            RemoveNode(nodeItem);
            itemsToDelete.removeOne(item);
        }
    }
}
