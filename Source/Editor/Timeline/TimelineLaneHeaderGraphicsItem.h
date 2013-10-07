/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineLaneHeaderGraphicsItem.h
//! \author	Kevin Boulanger
//! \date	29th September 2013
//! \brief	Graphics item representing the header block on the left of a lane in the timeline

#ifndef EDITOR_TIMELINELANEHEADERGRAPHICSITEM_H
#define EDITOR_TIMELINELANEHEADERGRAPHICSITEM_H

#include <QGraphicsItem>


//! Graphics item representing the header block on the left of a lane in the timeline
class TimelineLaneHeaderGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor
    //! \param lane Initial lane index
    TimelineLaneHeaderGraphicsItem(unsigned int lane);

    //! Destructor
    virtual ~TimelineLaneHeaderGraphicsItem();

    //! Set the lane index (typically happens when removing or inserting lanes around)
    //! \param lane Initial lane index
    void SetLane(unsigned int lane);

    //! Set the name of the lane
    //! \param name New name of the lane
    void SetName(const QString & name);

    //! Set the header color (before being lightened internally)
    //! \param color Color associated with the lane
    void SetColor(const QColor & color);


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 5 };
    int type() const { return Type; }


private:

    //! Update the position from the lane index
    void SetPositionFromLane();

    //! Compute the colors of the header from a given base color
    //! \param color Base color before being lightened
    //! \note Updates \a mTopColor and \a mBottomColor
    void ComputeColorsFromBase(const QColor & color);


    //! Index of the lane associated with the header
    unsigned int mLane;

    //! Name of the lane ("Lane X" when created)
    QString mName;

    //! Header color at the top of the gradient (after being lightened)
    QColor mTopColor;

    //! Header color at the bottom of the gradient (after being lightened then darkened a bit)
    QColor mBottomColor;
};


#endif  // EDITOR_TIMELINELANEHEADERGRAPHICSITEM_H
