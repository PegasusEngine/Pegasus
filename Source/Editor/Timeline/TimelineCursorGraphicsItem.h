/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineCursorGraphicsItem.h
//! \author	Kevin Boulanger
//! \date	27th September 2013
//! \brief	Graphics item representing the currently selected time as a vertical bar on top of the timeline

#ifndef EDITOR_TIMELINECURSORGRAPHICSITEM_H
#define EDITOR_TIMELINECURSORGRAPHICSITEM_H

#include <QGraphicsItem>


//! Graphics item representing the currently selected time as a vertical bar on top of the timeline
class TimelineCursorGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor, places the cursor at the beginning of the timeline
    //! \param numLanes Number of lanes (>= 1)
    //! \param horizontalScale Horizontal scale of the timeline, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    TimelineCursorGraphicsItem(unsigned int numLanes, float horizontalScale);

    //! Destructor
    ~TimelineCursorGraphicsItem();

    //! Set the position of the cursor
    //! \param beat Position of the cursor, expressed in beats,
    //!             with the possibility of fractional values (>= 0.0f)
    void SetBeat(float beat);

    //! Get the position of the cursor
    //! \return Position of the cursor, expressed in beats,
    //!         with the possibility of fractional values (>= 0.0f)
    inline float GetBeat() const { return mBeat; }

    //! Set the number of lanes of the timeline
    //! \param numLanes New number of lanes (>= 1)
    void SetNumLanes(unsigned int numLanes);

    //! Set the horizontal scale of the timeline
    //! \param scale 1.0f for a 1:1 ratio, < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    void SetHorizontalScale(float scale);


    //! \todo Document those functions

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // Enable the use of qgraphicsitem_cast
    //! \todo Use a common repository of indexes to avoid conflicts
    //! \todo Document those two lines
    enum { Type = UserType + 20 };
    int type() const { return Type; }

private:

    //! Update the scaled position from the beat and the horizontal scale
    void SetPositionFromBeat();


    //! Position of the cursor, expressed in beats,
    //! with the possibility of fractional values (>= 0.0f)
    float mBeat;

    //! Number of lanes (>= 1)
    unsigned int mNumLanes;

    //! Horizontal scale of the timeline
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    float mHorizontalScale;
};

#endif // EDITOR_TIMELINECURSORGRAPHICSITEM_H
