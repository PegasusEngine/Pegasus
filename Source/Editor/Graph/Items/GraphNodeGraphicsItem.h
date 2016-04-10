/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphNodeGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	02nd April 2016
//! \brief	Graphics item representing one node in the graph

#ifndef EDITOR_GRAPHNODEGRAPHICSITEM_H
#define EDITOR_GRAPHNODEGRAPHICSITEM_H

#include <QGraphicsItem>

class QUndoStack;


//! Graphics item representing one node in the graph
class GraphNodeGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param undoStack Undo stack associated with the item
    GraphNodeGraphicsItem(QUndoStack* undoStack);

    //! Destructor
    virtual ~GraphNodeGraphicsItem();


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 100 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the node has been moved by the user
    void NodeMoved();

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

    //! Node ID counter, assigned uniquely to each block, used for merging undo commands
    //! (so moving a node does not create a new undo command for each pixel moved)
    static unsigned int sCurrentNodeID;
    
    //! Node ID, unique to each node, used for merging undo commands
    //! (so moving a node does not create a new undo command for each pixel moved)
    unsigned int mNodeID;

    //! Mouse click ID, unique for each time the click is maintained while moving a node.
    //! Used to create new undo commands each time the mouse click is released.
    static unsigned int sMouseClickID;
};


#endif // EDITOR_GRAPHNODEGRAPHICSITEM_H
