/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#ifndef PEGASUS_TIMELINE_TIMELINE_H
#define PEGASUS_TIMELINE_TIMELINE_H

#include "Pegasus/Timeline/Shared/TimelineDefs.h"

namespace Pegasus {
    namespace Timeline {
        class Lane;
        class TimelineProxy;
    }
}

namespace Pegasus {
namespace Timeline {


//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class Timeline
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    Timeline(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~Timeline();

    //! Clear the entire timeline and create a default lane
    void Clear();


    //! Set the speed of the timeline in beats per minute
    //! \param bpm Beats per minute (30.0f <= bpm <= 500.0f)
    void SetBeatsPerMinute(float bpm);

    //! Get the speed of the timeline in beats per minute
    //! \return Beats per minute (30.0f <= bpm <= 500.0f)
    inline float GetBeatsPerMinute() const { return mBeatsPerMinute; }


    //! Set the length of the timeline
    //! \warning If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted
    //! \param numBeats Number of beats defining the new length of the timeline (> 0)
    void SetNumBeats(unsigned int numBeats);

    //! Get the length of the timeline
    //! \return Number of beats defining the length of the timeline (>= 1)
    inline unsigned int GetNumBeats() const { return mNumBeats; }


    //! Maximum number of lanes allowed in the timeline
    enum { MAX_NUM_LANES = 64 };

    //! Create a new lane
    //! \return New lane, nullptr if the number of lanes is MAX_NUM_LANES
    Lane * CreateLane();

    //! Get the current number of lanes
    //! \return Number of lanes (>= 1)
    inline unsigned int GetNumLanes() const { return mNumLanes; }

    //! Get one of the lanes
    //! \param laneIndex Index of the lane (< GetNumLanes())
    //! \return Pointer to the lane, nullptr in case of error
    Lane * GetLane(unsigned int laneIndex) const;


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    void SetPlayMode(PlayMode playMode);

    //! Update the current state of the timeline based on the play mode and the current time
    void Update();

    //! Set the current beat of the timeline
    //! \param beat Current beat, can have fractional part
    void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, can have fractional part
    inline float GetCurrentBeat() const { return mCurrentBeat; }


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the timeline
    inline TimelineProxy * GetProxy() const { return mProxy; }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // The timeline cannot be copied
    PG_DISABLE_COPY(Timeline)

        
    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the timeline
    TimelineProxy * mProxy;

    //! True when the real-time play mode has been selected, to update mStartPegasusTime on the next call to Update()
    bool mRequiresStartTimeComputation;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Speed of the timeline in beats per minute (30.0f <= bpm <= 500.0f)
    float mBeatsPerMinute;

    //! Number of beats defining the length of the timeline (>= 1)
    unsigned int mNumBeats;

    //! Set of lanes, only the first mNumLanes are defined
    Lane * mLanes[MAX_NUM_LANES];

    //! Number of used lanes (<= MAX_NUM_LANES)
    unsigned int mNumLanes;

    //! Current play mode of the timeline (PLAYMODE_xxx constant, PLAYMODE_REALTIME by default)
    PlayMode mPlayMode;

    //! Current beat of the timeline (>= 0.0f, integer for each beat, can have fractional part).
    //! INVALID_BEAT before the first call to Update()
    float mCurrentBeat;

    //! Invalid beat, used to detect the Update() function has not been called yet
    static const float INVALID_BEAT;

    //! Pegasus time returned by \a GetPegasusTime() when the timeline was started being played
    double mStartPegasusTime;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
