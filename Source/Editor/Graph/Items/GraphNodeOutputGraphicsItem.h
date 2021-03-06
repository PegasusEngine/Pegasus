/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeOutputGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	17th April 2016
//! \brief	Graphics item representing one node output in the graph

#ifndef EDITOR_GRAPHNODEOUTPUTGRAPHICSITEM_H
#define EDITOR_GRAPHNODEOUTPUTGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QList>

class QUndoStack;
class GraphConnectionGraphicsItem;


//! Graphics item representing one node output in the graph
class GraphNodeOutputGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param scene Scene the item will belong to
    //! \param undoStack Undo stack associated with the item
    //! \warning Use scene for access to the scene, and not scene(),
    //!          as it is not defined yet in the constructor
    GraphNodeOutputGraphicsItem(QGraphicsScene* scene, QUndoStack* undoStack);

    //! Destructor
    virtual ~GraphNodeOutputGraphicsItem();

    //! Add a connection linked to this output
    //! \param connectionItem Connection graphics item to add
    void AddConnection(GraphConnectionGraphicsItem* connectionItem);

    //! Disconnect a connection linked to this output
    //! \warning Does not delete the connection item, only removes the weak pointer
    //! \param connectionItem Connection graphics item to remove
    void DisconnectConnection(GraphConnectionGraphicsItem* connectionItem);

    //! Get the number of connections linked to this output
    //! \return Number of connections linked to this output (>= 0)
    inline unsigned int GetNumConnections() const { return mConnectionItems.size(); }

    //! Get one of the connections linked to this output
    //! \param index Index of the connection (0 <= index < GetNumConnections())
    //! \return Connection linked to this output
    inline GraphConnectionGraphicsItem* GetConnection(unsigned int index) const { return mConnectionItems[index]; }


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 102 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the user starts a new floating connection
    void StartFloatingDstConnection(GraphNodeOutputGraphicsItem* outputItem,
                                    QGraphicsSceneMouseEvent* mouseEvent);

    //! Emitted when the user releases the current floating connection
    void EndFloatingDstConnection(GraphNodeOutputGraphicsItem* outputItem,
                                  QGraphicsSceneMouseEvent* mouseEvent);

    //! Emitted when the node has been moved by the user
    //void NodeMoved();

    //! Emitted when the user does a double click to this node
    //void DoubleClicked(Pegasus::Timeline::IBlockProxy * block);

    //------------------------------------------------------------------------------------
    
protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent);

    //------------------------------------------------------------------------------------
    
private:

    //! Undo stack associated with this item
    QUndoStack* mUndoStack;

    //! True if undo commands can be sent
    bool mEnableUndo;

    //! Connection graphics items linked to this output
    QList<GraphConnectionGraphicsItem*> mConnectionItems;

    //! Node ID counter, assigned uniquely to each block, used for merging undo commands
    //! (so moving a node does not create a new undo command for each pixel moved)
    //static unsigned int sCurrentNodeID;
    
    //! Node ID, unique to each node, used for merging undo commands
    //! (so moving a node does not create a new undo command for each pixel moved)
    //unsigned int mNodeID;

    //! Mouse click ID, unique for each time the click is maintained while moving a node.
    //! Used to create new undo commands each time the mouse click is released.
    //static unsigned int sMouseClickID;
};


#endif // EDITOR_GRAPHNODEOUTPUTGRAPHICSITEM_H
