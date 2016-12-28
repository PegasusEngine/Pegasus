/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphConnectionGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	03rd May 2016
//! \brief	Graphics item representing a connection between two nodes in the graph

#ifndef EDITOR_GRAPHCONNECTIONGRAPHICSITEM_H
#define EDITOR_GRAPHCONNECTIONGRAPHICSITEM_H

#include <QGraphicsItem>

class QUndoStack;
class GraphNodeInputGraphicsItem;
class GraphNodeOutputGraphicsItem;


//! Graphics item representing a connection between two nodes in the graph
class GraphConnectionGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

    // The constructor and destructor are private. The only way to create or remove a connection
    // is through GraphEditorGraphicsScene

private:

    friend class GraphEditorGraphicsScene;

    //! Constructor
    //! \param scene Scene the item will belong to
    //! \param srcOutput Node output that is the source of the connection,
    //!                 or nullptr for a floating connection
    //! \param dstInput Node input that is the destination of the connection,
    //!                 or nullptr for a floating connection
    //! \param undoStack Undo stack associated with the item
    //! \warning Use scene for access to the scene, and not scene(),
    //!          as it is not defined yet in the constructor
    GraphConnectionGraphicsItem(GraphNodeOutputGraphicsItem* srcOutput,
                                GraphNodeInputGraphicsItem* dstInput,
                                QGraphicsScene* scene,
                                QUndoStack* undoStack);

    //! Destructor
    virtual ~GraphConnectionGraphicsItem();

    //! Test if the connection is floating
    //! \return True if either the source output or the destination input is undefined
    inline bool IsFloating() const { return (mSrcOutput == nullptr) || (mDstInput == nullptr); }

    //------------------------------------------------------------------------------------
    
public:

    //! Get the node output that is the source of the connection
    //! \return Pointer to the node output that is the source of the connection,
    //!         nullptr for a floating connection
    inline GraphNodeOutputGraphicsItem* GetSrcOutput() const { return mSrcOutput; }

    //! Get the node input that is the destination of the connection
    //! \return Pointer to the node input that is the destination of the connection,
    //!         nullptr for a floating connection
    inline GraphNodeInputGraphicsItem* GetDstInput() const { return mDstInput; }

    //! Call when the source output or destination input are being moved,
    //! to update the connection's geometry and ask for a redraw
    void AdjustGeometry();

    //! Call when the start point is being moved while floating,
    //! to update the connection's geometry and ask for a redraw
    //! \param srcOutputPos Scene position of the current start point of the connection when floating
    void AdjustSrcGeometry(QPointF srcOutputPos);

    //! Call when the end point is being moved while floating,
    //! to update the connection's geometry and ask for a redraw
    //! \param dstInputPos Scene position of the current end point of the connection when floating
    void AdjustDstGeometry(QPointF dstInputPos);

    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 110 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the connection has been moved by the user
    //void NodeMoved();

    //! Emitted when the user does a double click to this node
    //void DoubleClicked(Pegasus::Timeline::IBlockProxy * block);

    //------------------------------------------------------------------------------------
    
protected:

    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //------------------------------------------------------------------------------------
    
private:

    //! Node output that is the source of the connection (!= nullptr)
    GraphNodeOutputGraphicsItem* mSrcOutput;

    //! Node input that is the destination of the connection.
    //! nullptr for a floating connection
    GraphNodeInputGraphicsItem* mDstInput;


    //! Undo stack associated with this item
    QUndoStack* mUndoStack;

    //! True if undo commands can be sent
    bool mEnableUndo;

    ////! Node ID counter, assigned uniquely to each block, used for merging undo commands
    ////! (so moving a node does not create a new undo command for each pixel moved)
    //static unsigned int sCurrentNodeID;
    //
    ////! Node ID, unique to each node, used for merging undo commands
    ////! (so moving a node does not create a new undo command for each pixel moved)
    //unsigned int mNodeID;

    ////! Mouse click ID, unique for each time the click is maintained while moving a node.
    ////! Used to create new undo commands each time the mouse click is released.
    //static unsigned int sMouseClickID;

    //! Scene position of the current start point of the connection when floating
    QPointF mFloatingSrcOutputPos;

    //! Scene position of the current end point of the connection when floating
    QPointF mFloatingDstInputPos;
};


#endif // EDITOR_GRAPHCONNECTIONGRAPHICSITEM_H
