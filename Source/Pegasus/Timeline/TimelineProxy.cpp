/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline

//! \todo Why do we need this in Rel-Debug? TimelineProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/TimelineProxy.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/LaneProxy.h"

namespace Pegasus {
namespace Timeline {


TimelineProxy::TimelineProxy(Timeline * timeline)
:   mTimeline(timeline)
{
    PG_ASSERTSTR(timeline != nullptr, "Trying to create a timeline proxy from an invalid timeline object");
}

//----------------------------------------------------------------------------------------

TimelineProxy::~TimelineProxy()
{
}

//----------------------------------------------------------------------------------------

void TimelineProxy::SetBeatsPerMinute(float bpm)
{
    mTimeline->SetBeatsPerMinute(bpm);
}

//----------------------------------------------------------------------------------------

float TimelineProxy::GetBeatsPerMinute() const
{
    return mTimeline->GetBeatsPerMinute();
}
    
//----------------------------------------------------------------------------------------

void TimelineProxy::SetNumBeats(unsigned int numBeats)
{
    mTimeline->SetNumBeats(numBeats);
}

//----------------------------------------------------------------------------------------

unsigned int TimelineProxy::GetNumBeats() const
{
    return mTimeline->GetNumBeats();
}

//----------------------------------------------------------------------------------------

ILaneProxy * TimelineProxy::CreateLane()
{
    Lane * const lane = mTimeline->CreateLane();
    if (lane != nullptr)
    {
        return lane->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

unsigned int TimelineProxy::GetNumLanes() const
{
    return mTimeline->GetNumLanes();
}

//----------------------------------------------------------------------------------------

ILaneProxy * TimelineProxy::GetLane(unsigned int laneIndex) const
{
    Lane * const lane = mTimeline->GetLane(laneIndex);
    if (lane != nullptr)
    {
        return lane->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------
    
void TimelineProxy::SetPlayMode(PlayMode playMode)
{
    mTimeline->SetPlayMode(playMode);
}

//----------------------------------------------------------------------------------------

void TimelineProxy::Update()
{
    mTimeline->Update();
}

//----------------------------------------------------------------------------------------

void TimelineProxy::SetCurrentBeat(float beat)
{
    mTimeline->SetCurrentBeat(beat);
}

//----------------------------------------------------------------------------------------

float TimelineProxy::GetCurrentBeat() const
{
    return mTimeline->GetCurrentBeat();
}


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
