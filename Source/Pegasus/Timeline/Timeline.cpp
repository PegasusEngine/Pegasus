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
:   mNumLanes(0),
    mAllocator(allocator),
    mPlayMode(PLAYMODE_REALTIME),
    mCurrentBeat(INVALID_BEAT),
    mStartPegasusTime(0.0)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(allocator, -1, "Timeline::mProxy", Pegasus::Alloc::PG_MEM_PERM) TimelineProxy(this);
#endif  // PEGASUS_ENABLE_PROXIES
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
            const double currentTime = Core::GetPegasusTime() - mStartPegasusTime;
            //! \todo Implement tempo
            mCurrentBeat = static_cast<float>(currentTime / /*mBeatsPerSecond*/2.0);
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
}


}   // namespace Timeline
}   // namespace Pegasus
