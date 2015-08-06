/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBackgroundBeatLineGraphicsItem.h
//! \author	Karolyn Boulanger
//! \date	23rd July 2013
//! \brief	Graphics item representing a vertical line for a beat of the background in the timeline

#ifndef EDITOR_TIMELINEBACKGROUNDBEATLINEGRAPHICSITEM_H
#define EDITOR_TIMELINEBACKGROUNDBEATLINEGRAPHICSITEM_H

#include <QGraphicsItem>


//! Graphics item representing one beat of the timeline's background,
//! the line is displayed at the beginning of the beat space
class TimelineBackgroundBeatLineGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor
    //! \param beat Index of the beat (>= 1)
    //! \param numLanes Number of lanes (>= 1)
    //! \param horizontalScale Horizontal scale of the timeline, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    TimelineBackgroundBeatLineGraphicsItem(unsigned int beat,
                                           unsigned int numLanes,
                                           float horizontalScale);

    //! Destructor
    virtual ~TimelineBackgroundBeatLineGraphicsItem();

    //! Set the number of lanes of the background item
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
    enum { Type = UserType + 2 };
    int type() const { return Type; }


private:

    //! Update the scaled position from the beat and the horizontal scale
    void SetPositionFromBeat();


    //! Index of the beat (>= 1)
    const unsigned int mBeat;

    //! Number of lanes (>= 1)
    unsigned int mNumLanes;

    //! Horizontal scale of the timeline
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    float mHorizontalScale;
};


#endif  // EDITOR_TIMELINEBACKGROUNDBEATLINEGRAPHICSITEM_H
