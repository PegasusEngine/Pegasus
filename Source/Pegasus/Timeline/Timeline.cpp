/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Core/Time.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/TimelineProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


const float Timeline::INVALID_BEAT = -1.0f;

//----------------------------------------------------------------------------------------

Timeline::Timeline(Alloc::IAllocator * allocator)
:   mBeatsPerMinute(120.0f),
    mNumBeats(128),
    mNumLanes(0),
    mAllocator(allocator),
    mPlayMode(PLAYMODE_REALTIME),
    mCurrentBeat(INVALID_BEAT),
    mStartPegasusTime(0.0)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(allocator, -1, "Timeline::mProxy", Pegasus::Alloc::PG_MEM_PERM) TimelineProxy(this);

    mRequiresStartTimeComputation = false;
#endif  // PEGASUS_ENABLE_PROXIES

    // Create the initial default lane
    Clear();
}

//----------------------------------------------------------------------------------------

Timeline::~Timeline()
{
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }

#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the timeline
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

void Timeline::Clear()
{
    // Delete the existing lanes
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }
    mNumLanes = 0;

    // Create a default lane
    CreateLane();

    //! \todo Remove those temporary lanes
    /****/CreateLane();
    /****/CreateLane();
    /****/CreateLane();
    /****/CreateLane();
}

//----------------------------------------------------------------------------------------

void Timeline::SetBeatsPerMinute(float bpm)
{
    if (bpm < 30.0f)
    {
        PG_FAILSTR("Invalid speed for the timeline (%f BPM). It should be >= 30.0f", bpm);
        mBeatsPerMinute = 30.0f;
    }
    else if (bpm > 500.0f)
    {
        PG_FAILSTR("Invalid speed for the timeline (%f BPM). It should be <= 500.0f", bpm);
        mBeatsPerMinute = 500.0f;
    }
    else
    {
        mBeatsPerMinute = bpm;
    }

#if PEGASUS_ENABLE_PROXIES
    // If the play mode is real-time, the Pegasus start time needs to be recomputed
    // to avoid the cursor jumping all over the place
    if (mPlayMode == PLAYMODE_REALTIME)
    {
        mRequiresStartTimeComputation = true;
    }
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

void Timeline::SetNumBeats(unsigned int numBeats)
{
    if (numBeats >= 1)
    {
        //! \todo If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted

        mNumBeats = numBeats;
    }
    else
    {
        PG_FAILSTR("Invalid number of beats (%d) to define the length of the timeline");
    }
}

//----------------------------------------------------------------------------------------

Lane * Timeline::CreateLane()
{
    PG_LOG('TMLN', "Creating a new lane (index %d)", mNumLanes);

    if (mNumLanes < MAX_NUM_LANES)
    {
        mLanes[mNumLanes] = PG_NEW(mAllocator, -1, "Timeline::Lane", Alloc::PG_MEM_PERM) Lane(mAllocator);
        ++mNumLanes;

        return mLanes[mNumLanes - 1];
    }
    else
    {
        PG_FAILSTR("Unable to create a new lane, the maximum number of lane has been reached %d", MAX_NUM_LANES);
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Timeline::Update()
{
    if ((mCurrentBeat == INVALID_BEAT) || (mCurrentBeat < 0.0f))
    {
        mCurrentBeat = 0.0f;
        Core::UpdatePegasusTime();
        mStartPegasusTime = Core::GetPegasusTime();
        mCurrentBeat = 0.0f;
    }
    else
    {
        if (mPlayMode == PLAYMODE_REALTIME)
        {
            Core::UpdatePegasusTime();

#if PEGASUS_ENABLE_PROXIES
            // If the start time needs to be recomputed (typically when the cursor position is changed,
            // or when the play mode has just been set to real-time)
            if (mRequiresStartTimeComputation)
            {
                // Current time that we should have got from the timer for the current beat and the current tempo
                double requiredCurrentTime = static_cast<double>(mCurrentBeat / (mBeatsPerMinute * (1.0f / 60.0f)));

                // Move the start Pegasus time to simulate a non-stop execution of the timeline until the current beat
                mStartPegasusTime = static_cast<float>(Core::GetPegasusTime() - requiredCurrentTime);

                mRequiresStartTimeComputation = false;
            }
#endif  // PEGASUS_ENABLE_PROXIES

            const double currentTime = Core::GetPegasusTime() - mStartPegasusTime;
            mCurrentBeat = static_cast<float>(currentTime * (mBeatsPerMinute * (1.0f / 60.0f)));
        }
    }
}

//----------------------------------------------------------------------------------------

void Timeline::SetPlayMode(PlayMode playMode)
{
    if (playMode < NUM_PLAYMODES)
    {
        switch (playMode)
        {
            case PLAYMODE_REALTIME:
                PG_LOG('TMLN', "Switched to real-time mode for the timeline");
#if PEGASUS_ENABLE_PROXIES
                // The Pegasus start time needs to be recomputed to avoid the cursor jumping all over the place
                mRequiresStartTimeComputation = true;
#endif 
                break;

            case PLAYMODE_STOPPED:
                PG_LOG('TMLN', "Switched to stopped mode for the timeline");
                break;

            default:
                PG_LOG('TMLN', "Switched to an unknown mode for the timeline");
        }

        mPlayMode = playMode;
    }
    else
    {
        PG_FAILSTR("Invalid play mode (%d) for the timeline. The maximum value is %d", playMode, NUM_PLAYMODES - 1);
    }
}

//----------------------------------------------------------------------------------------

void Timeline::SetCurrentBeat(float beat)
{
    //! \todo Implement proper behavior (more safety, handles play modes)
    mCurrentBeat = beat;

    PG_LOG('TMLN', "Set the current beat of the timeline to %f", mCurrentBeat);

#if PEGASUS_ENABLE_PROXIES
    // If the play mode is real-time, the Pegasus start time needs to be recomputed
    if (mPlayMode == PLAYMODE_REALTIME)
    {
        mRequiresStartTimeComputation = true;
    }
#endif  // PEGASUS_ENABLE_PROXIES
}


}   // namespace Timeline
}   // namespace Pegasus
