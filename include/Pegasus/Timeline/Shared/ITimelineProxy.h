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


    //! Set the speed of the timeline in beats per minute
    //! \param bpm Beats per minute (30.0f <= bpm <= 500.0f)
    virtual void SetBeatsPerMinute(float bpm) = 0;

    //! Get the speed of the timeline in beats per minute
    //! \return Beats per minute (30.0f <= bpm <= 500.0f)
    virtual float GetBeatsPerMinute() const = 0;


    //! Set the length of the timeline
    //! \warning If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted
    //! \param numBeats Number of beats defining the new length of the timeline (> 0)
    virtual void SetNumBeats(unsigned int numBeats) = 0;

    //! Get the length of the timeline
    //! \return Number of beats defining the length of the timeline (>= 1)
    virtual unsigned int GetNumBeats() const = 0;


    //! Create a new lane
    //! \return Proxy interface for a timeline lane, nullptr if the number of lanes is Timeline::TIMELINE_MAX_NUM_LANES
    virtual ILaneProxy * CreateLane() = 0;

    //! Get the current number of lanes
    //! \return Number of lanes (>= 1)
    virtual unsigned int GetNumLanes() const = 0;

    //! Get one of the lanes
    //! \param laneIndex Index of the lane (< GetNumLanes())
    //! \return Pointer to the lane proxy, nullptr in case of error
    virtual ILaneProxy * GetLane(unsigned int laneIndex) const = 0;


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    virtual void SetPlayMode(PlayMode playMode) = 0;

    //! Update the current state of the timeline based on the play mode and the current time
    virtual void Update() = 0;

    //! Set the current beat of the timeline
    //! \param beat Current beat, can have fractional part
    virtual void SetCurrentBeat(float beat) = 0;

    //! Get the current beat of the timeline
    //! \return Current beat, can have fractional part
    virtual float GetCurrentBeat() const = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITIMELINEPROXY_H
