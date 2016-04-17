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
class GraphNodeInputGraphicsItem;
class GraphNodeOutputGraphicsItem;
class QGraphicsSimpleTextItem;


//! Graphics item representing one node in the graph
class GraphNodeGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param scene Scene the item will belong to
    //! \param title Name that appears in the header of the node
    //! \param undoStack Undo stack associated with the item
    //! \warning Use scene for access to the scene, and not scene(),
    //!          as it is not defined yet in the constructor
    GraphNodeGraphicsItem(const QString& title,
                          QGraphicsScene* scene,
                          QUndoStack* undoStack);

    //! Destructor
    virtual ~GraphNodeGraphicsItem();

    //! Add an input to the node
    //! \param name Name of the input, needs to be short to fit in the node item,
    //!             starts with an uppercase letter
    //! \warning The node item has to be added to the scene before this function can be called
    void AddInput(const QString& name);

    //! Get the current number of inputs of the node item
    //! \return Current number of inputs
    inline unsigned int GetNumInputs() const { return static_cast<unsigned int>(mInputs.size()); }

    //! Get one of the input items
    //! \param Index of the input (0 <= index < GetNumInputs())
    //! \return Pointer to the input item if the index is valid, nullptr otherwise
    const GraphNodeInputGraphicsItem* GetInputItem(unsigned int index) const;


    //! Get the output item
    //! \return Pointer to the output item
    const GraphNodeOutputGraphicsItem* GetOutputItem() const;


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

    //! Get the height of the node body depending on the number of inputs
    //! (from the bottom of the header to the top of the footer)
    //! \return Height of the node body in pixels
    float GetBodyHeight() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //------------------------------------------------------------------------------------
    
private:

    //! Name that appears in the header of the node
    const QString mTitle;


    //! Description of an input
    struct Input
    {
        //! Name of the input, needs to be short to fit in the node item, starts with an uppercase letter
        QString name;       

        //! Child item representing an input connector
        GraphNodeInputGraphicsItem* item;

        //! Child item representing the text next to the input connector
        QGraphicsSimpleTextItem* labelItem; 
    };

    //! List of inputs used by the node
    QList<Input> mInputs;


    //! Description of an output
    struct Output
    {
        //! Child item representing an output connector
        GraphNodeOutputGraphicsItem* item;
    };

    //! Output used by the node
    Output mOutput;


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
