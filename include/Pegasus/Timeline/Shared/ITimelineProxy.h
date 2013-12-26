/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITimelineProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline

#ifndef PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/TimelineDefs.h"

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline
class ITimelineProxy
{
public:

    //! Destructor
    virtual ~ITimelineProxy() {};


    //! Create a new lane
    //! \return Proxy interface for a timeline lane, nullptr if the number of lanes is Timeline::MAX_NUM_LANES
    virtual ILaneProxy * CreateLane() = 0;


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    virtual void SetPlayMode(PlayMode playMode) = 0;

    //! Update the current state of the timeline based on the play mode and the current time
    virtual void Update() = 0;

    //! Set the current beat of the timeline
    //! \param beat Current beat, can have fractional part
    virtual void SetCurrentBeat(float beat) = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H
