/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineBackgroundBeatGraphicsItem.h
//! \author	Kevin Boulanger
//! \date	21st July 2013
//! \brief	Graphics item representing one beat of the background in the timeline

#ifndef EDITOR_TIMELINEBACKGROUNDBEATGRAPHICSITEM_H
#define EDITOR_TIMELINEBACKGROUNDBEATGRAPHICSITEM_H

#include <QGraphicsItem>


//! Graphics item representing one beat of the timeline's background
class TimelineBackgroundBeatGraphicsItem : public QGraphicsItem
{
public:

    //! Constructor
    //! \param beat Index of the beat (0-based)
    //! \param numLanes Number of lanes (>= 1)
    //! \param horizontalScale Horizontal scale of the timeline, 1.0f for a 1:1 ratio,
    //!                        < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    TimelineBackgroundBeatGraphicsItem(unsigned int beat,
                                       unsigned int numLanes,
                                       float horizontalScale);

    //! Destructor
    virtual ~TimelineBackgroundBeatGraphicsItem();

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
    enum { Type = UserType + 1 };
    int type() const { return Type; }


private:

    //! Update the scaled position from the beat and the horizontal scale
    void SetPositionFromBeat();


    //! Index of the beat (0-based, integer cast as float)
    const float mBeat;
    
    //! Number of lanes (>= 1)
    unsigned int mNumLanes;

    //! Horizontal scale of the timeline
    //! 1.0f for a 1:1 ratio, < 1.0f for a compressed timeline, > 1.0f for an expanded timeline
    float mHorizontalScale;
};


#endif  // EDITOR_TIMELINEBACKGROUNDBEATGRAPHICSITEM_H
