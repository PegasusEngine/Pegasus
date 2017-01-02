/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBlockGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	11th June 2013
//! \brief	Graphics item representing one block in the timeline

#ifndef EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
#define EDITOR_TIMELINEBLOCKGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QIcon>

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/TimelineIOMessageController.h"

class QUndoStack;
class QMenu;
class QAction;


//! Graphics item representing one timeline block.
//! Contains the block rendering properties and the rendering code.
class TimelineBlockGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:

    //! Constructor
    //! \param blockProxy Block proxy associated with the item
    //! \param lane Index of the timeline lane the block belongs to
    //! \param horizontalScale Horizontal scale of the block, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed block, > 1.0f for an expanded block
    //! \param undoStack Undo stack associated with the item
    TimelineBlockGraphicsItem(const ShadowBlockState& blockState,
                              const ShadowTimelineState* timelineState,
                              unsigned int lane,
                              float horizontalScale,
                              QUndoStack* undoStack);

    //! Destructor
    virtual ~TimelineBlockGraphicsItem();

    //! Get the block proxy associated with the item
    //! \return Block proxy associated with the item (!= nullptr)
    inline const ShadowBlockState& GetBlockProxy() const { return mBlockState; }

    //! Get the block shadow state associated with the item for edit. 
    //! \return Block proxy associated with the item. 
    inline ShadowBlockState& EditBlockProxy() {
        return mBlockState;
    }

    //! Set the timeline lane of the block
    //! \param lane Index of the timeline lane the block belongs to
    void SetLane(unsigned int lane);

    //! Get the timeline lane of the block
    //! \return Index of the timeline lane the block belongs to
    inline unsigned int GetLane() const { return mLane; }

    //! Set the position of the block
    //! \param beat Position of the block, measured in ticks
    void SetBeat(Pegasus::Timeline::Beat beat, bool updateItem = true);

    //! Get the position of the block
    //! \return Position of the block, measured in ticks
    inline Pegasus::Timeline::Beat GetBeat() const { return mBeat; }

    //! Set the duration of the block
    //! \param duration Duration of the block, measured in ticks (> 0)
    //! \param updateItem True if the graphics item needs to be updated (use false only for special cases)
    void SetDuration(Pegasus::Timeline::Duration duration, bool updateItem = true);

    //! Get the duration of the block
    //! \return Duration of the block, measured in ticks (> 0)
    inline Pegasus::Timeline::Duration GetDuration() const { return mDuration; }

    //! Set the horizontal scale of the block
    //! \param scale 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    void SetHorizontalScale(float scale);

    //! Get the horizontal scale of the block
    //! \return 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    //inline float GetHorizontalScale() const { return mHorizontalScale; }

    //! updates ui shadow state to display the script updated.
    void UIUpdateBlockScript(const QString& script);

    //! updates ui shadow state to clear the blockscript ui.
    void UIClearBlockScript();


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //! Update the scaled length from the duration in ticks
    Pegasus::Timeline::Beat GetBeatFromX(float x) const;

    //! Update the scaled length from the duration in ticks
    unsigned int GetLaneFromY(float lanePosition) const;
    
    //! Mouse click ID, unique for each time the click is maintained while moving a block.
    //! Used to create new undo commands each time the mouse click is released.
    static unsigned int sMouseClickID;

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 10 };
    int type() const { return Type; }

    //------------------------------------------------------------------------------------

signals:

    //! Emitted when the user does a double click to this block
    void DoubleClicked(QString blockScriptToOpen);

    //! Request this script to be changed for the block with the guid.
    void RequestChangeScript(QGraphicsObject* sender, unsigned blockId);

    //! Request the blockscript to be removed
    void RequestRemoveScript(QGraphicsObject* sender, unsigned blockId);

    //! Requests this block move.
    void RequestBlockMove(QGraphicsObject* sender, QPointF amount);

    //! Requests draw all viewports
    void RequestDrawAllViewports();


public slots:

    //! Flushes visual properties from a timeline block into this graphics item.
    void FlushVisualProperties();

    //! Asks the user to attach a new script
    void CtxMenuChangeScript();

    //! Removes a script for this block executed from ctx menu
    void CtxRemoveScript();
    
protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //------------------------------------------------------------------------------------

private:

    //! Update the scaled position in pixels from the position in ticks
    void SetXFromBeat();

    //! Update the vertical position from the lane index
    void SetYFromLane();

    //! Update the scaled length from the duration in ticks
    void SetLengthFromDuration();

    //! Block proxy associated with the item
    ShadowBlockState mBlockState;
    
    //! Parent timeline state 
    const ShadowTimelineState* mTimelineState;

    //! Handle of the parent timeline state
    AssetInstanceHandle mTimelineHandle;

    //! Index of the timeline lane the block belongs to
    unsigned int mLane;
    
    //! Name of the block, displayed as text on top of the block
    QString mName;

    //! Name of the block, displayed as text on top of the block
    QString mClassName;

    //! Base color of the block, when not selected
    QColor mBaseColor;

    //! Position of the block in the lane, measured in ticks
    Pegasus::Timeline::Beat mBeat;

    //! Duration of the block, measured in ticks (> 0)
    Pegasus::Timeline::Duration mDuration;

    //! Horizontal scale of the block
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed block, > 1.0f for an expanded block
    float mHorizontalScale;

    //! Position in pixels (with no zoom applied, but horizontal scale applied)
    float mX;

    //! Vertical position in pixels (with no zoom applied)
    float mY;

    //! Length in pixels (with no zoom applied, but horizontal scale applied)
    float mLength;


    //! Undo stack associated with this item
    QUndoStack* mUndoStack;

    //! True if undo commands can be sent
    bool mEnableUndo;

    //! Block ID counter, assigned uniquely to each block, used for merging undo commands
    //! (so moving a block does not create a new undo command for each pixel moved)
    static unsigned int sCurrentBlockID;
    
    //! Block ID, unique to each block, used for merging undo commands
    //! (so moving a block does not create a new undo command for each pixel moved)
    unsigned int mBlockID;

    //! Context menu on right click.
    QMenu* mCtxMenu;
    QMenu* mNoScriptCtxMenu;
    QAction* mChangeScriptAction;
    
    //! Flag that determines if this block has a script.
    bool mHasScript;

    //! Icon of a script
    QIcon mScriptIcon;

    //! Queue of candidate positions to move this block.
    QVector<QPointF> mPositionQueue;

    bool mIsMoving;
    QPointF mInitialPoint;
    QPointF mInitialMouse;
};


#endif  // EDITOR_TIMELINEBLOCKGRAPHICSITEM_H
