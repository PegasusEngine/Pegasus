/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline

#ifndef PEGASUS_TEXTURE_TIMELINEPROXY_H
#define PEGASUS_TEXTURE_TIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"

namespace Pegasus {
    namespace Timeline {
        class Timeline;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy object, used by the editor to interact with the timeline
class TimelineProxy : public ITimelineProxy
{
public:

    // Constructor
    //! \param timeline Proxied timeline object, cannot be nullptr
    TimelineProxy(Timeline * timeline);

    //! Destructor
    virtual ~TimelineProxy();


    //! Create a new lane
    //! \return Proxy interface for a timeline lane, nullptr if the number of lanes is Timeline::MAX_NUM_LANES
    virtual ILaneProxy * CreateLane();


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    virtual void SetPlayMode(PlayMode playMode);

    //! Update the current state of the timeline based on the play mode and the current time
    virtual void Update();

    //! Set the current beat of the timeline
    //! \param beat Current beat, can have fractional part
    virtual void SetCurrentBeat(float beat);

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied timeline object
    Timeline * const mTimeline;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_TIMELINEPROXY_H
