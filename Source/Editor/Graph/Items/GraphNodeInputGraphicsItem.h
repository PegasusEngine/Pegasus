/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeInputGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	10th April 2016
//! \brief	Graphics item representing one node input in the graph

#ifndef EDITOR_GRAPHNODEINPUTGRAPHICSITEM_H
#define EDITOR_GRAPHNODEINPUTGRAPHICSITEM_H

#include <QGraphicsItem>

class QUndoStack;
class GraphConnectionGraphicsItem;


//! Graphics item representing one node input in the graph
class GraphNodeInputGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param scene Scene the item will belong to
    //! \param undoStack Undo stack associated with the item
    //! \warning Use scene for access to the scene, and not scene(),
    //!          as it is not defined yet in the constructor
    GraphNodeInputGraphicsItem(QGraphicsScene* scene, QUndoStack* undoStack);

    //! Destructor
    virtual ~GraphNodeInputGraphicsItem();

    //! Set the connection linked to this input
    //! \param connectionItem Connection graphics item to set, nullptr to remove the connection
    void SetConnection(GraphConnectionGraphicsItem* connectionItem);


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 101 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the node has been moved by the user
    //void NodeMoved();

    //! Emitted when the user does a double click to this node
    //void DoubleClicked(Pegasus::Timeline::IBlockProxy * block);

    //------------------------------------------------------------------------------------
    
protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //------------------------------------------------------------------------------------
    
private:

    //! Undo stack associated with this item
    QUndoStack* mUndoStack;

    //! True if undo commands can be sent
    bool mEnableUndo;

    //! Connection graphics item linked to this input (nullptr by default)
    GraphConnectionGraphicsItem* mConnectionItem;

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


#endif // EDITOR_GRAPHNODEINPUTGRAPHICSITEM_H
