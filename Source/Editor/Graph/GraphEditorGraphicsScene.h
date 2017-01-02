/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorGraphicsScene.h
//! \author	Karolyn Boulanger
//! \date	14th June 2016
//! \brief	Generic graphics scene for a graph editor (texture, mesh, etc.)

#ifndef EDITOR_GRAPHEDITORGRAPHICSSCENE_H
#define EDITOR_GRAPHEDITORGRAPHICSSCENE_H

#include <QAbstractScrollArea>
#include <QGraphicsScene>

class GraphNodeGraphicsItem;
class GraphConnectionGraphicsItem;
class GraphNodeInputGraphicsItem;
class GraphNodeOutputGraphicsItem;


//! Generic graphics scene for a graph editor (texture, mesh, etc.)
class GraphEditorGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:

    GraphEditorGraphicsScene(QObject* parent = 0);
    ~GraphEditorGraphicsScene();

    //! Scene mode, representing which user interaction is occurring within the scene
    enum SceneMode
    {
        SCENEMODE_IDLE,                         // Normal behavior when moving the mouse with no button clicked
        SCENEMODE_FLOATING_DST_CONNECTION,      // Currently creating a connection by dragging the mouse from a source output
        SCENEMODE_FLOATING_SRC_CONNECTION       // Currently creating a connection by dragging the mouse from a destination input
    };

    //! Get the current scene mode
    //! \note The mode is not a simple return of the current mode member, it is instead the result of an analysis
    //!       of the internal variables, to be canonical
    //! \return Current scene mode (SCENEMODE_xxx constant)
    SceneMode GetSceneMode() const;

    //! Create a new node item in the view
    //! \param title Name that appears in the header of the node
    //! \param inputList List of inputs for the new node
    //! \return Created node graphics item
    GraphNodeGraphicsItem* CreateNode(const QString& title, const QList<QString>& inputList);

    //! Remove a node item from the scene, and its associated connections
    //! \warning This is the only way to do it, the destructor of the connection item is private
    //! \param nodeItem Node item to remove (!= nullptr)
    void RemoveNode(GraphNodeGraphicsItem* nodeItem);

    //! Create a new connection item between a source node output and a destination node input
    //! \param srcOutput Output item of the source node
    //! \param dstInput One of the input items of the destination node
    //! \return Created connection graphics item, nullptr in case of error
    GraphConnectionGraphicsItem* CreateConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                                  GraphNodeInputGraphicsItem* dstInput);

    //! Remove a connection item from the scene
    //! \warning This is the only way to do it, the destructor of the connection item is private
    //! \param connectionItem Connection item to remove (!= nullptr)
    void RemoveConnection(GraphConnectionGraphicsItem* connectionItem);

    //------------------------------------------------------------------------------------
    
private slots:

    //! Create a new floating connection by dragging the mouse from a source output
    //! \param srcOutput Source output node of the connection that is started
    //! \param mouseEvent Information about the mouse when the button was pressed
    void StartFloatingDstConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                    QGraphicsSceneMouseEvent* mouseEvent);

    //! Release the current floating connection dragged from a source output
    //! \param srcOutput Source output node of the connection that is ended
    //! \param mouseEvent Information about the mouse when the button was released
    void EndFloatingDstConnection(GraphNodeOutputGraphicsItem* srcOutput,
                                  QGraphicsSceneMouseEvent* mouseEvent);

    //! Create a new floating connection by dragging the mouse from a destination input
    //! \param dstInput Destination input node of the connection that is started
    //! \param mouseEvent Information about the mouse when the button was pressed
    void StartFloatingSrcConnection(GraphNodeInputGraphicsItem* dstInput,
                                    QGraphicsSceneMouseEvent* mouseEvent);

    //! Release the current floating connection dragged from a destination input
    //! \param dstInput Destination input node of the connection that is ended
    //! \param mouseEvent Information about the mouse when the button was released
    void EndFloatingSrcConnection(GraphNodeInputGraphicsItem* dstInput,
                                  QGraphicsSceneMouseEvent* mouseEvent);

    //------------------------------------------------------------------------------------

protected:

    void keyPressEvent(QKeyEvent* keyEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);

    //------------------------------------------------------------------------------------
    
private:

    //! Get the item under the mouse cursor if it is an input item
    //! \param scenePos Position in scene space to be tested
    //! \return Pointer to the item if it is an input item, nullptr otherwise
    GraphNodeInputGraphicsItem* GetInputItemAtScenePos(const QPointF& scenePos) const;

    //! Get the item under the mouse cursor if it is an output item
    //! \param scenePos Position in scene space to be tested
    //! \return Pointer to the item if it is an output item, nullptr otherwise
    GraphNodeOutputGraphicsItem* GetOutputItemAtScenePos(const QPointF& scenePos) const;

    //! Delete the selected items, and if they contain nodes, the associated connections
    void DeleteSelectedItems();


    //! Current floating connection (with floating destination), so the mouse movements affect it,
    //! nullptr otherwise
    GraphConnectionGraphicsItem* mFloatingDstConnectionItem;

    //! Current floating connection (with floating source), so the mouse movements affect it,
    //! nullptr otherwise
    GraphConnectionGraphicsItem* mFloatingSrcConnectionItem;
};


#endif // EDITOR_GRAPHEDITORGRAPHICSSCENE_H
