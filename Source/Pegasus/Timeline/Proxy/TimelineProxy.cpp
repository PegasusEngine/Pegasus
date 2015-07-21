/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.cpp
//! \author	refactored by Kleber Garcia (original from Karolyn Boulanger)
//! \date	July 18, 2015
//! \brief	Timeline container, for lanes, and functions for playback

//! \todo Why do we need this in Rel-Debug? TimelineProxy should not even be compiled in REL mode
#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;

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

unsigned int TimelineProxy::GetNumTicksPerBeat() const
{
    return mTimeline->GetNumTicksPerBeat();
}

//----------------------------------------------------------------------------------------

float TimelineProxy::GetNumTicksPerBeatFloat() const
{
    return mTimeline->GetNumTicksPerBeatFloat();
}

//----------------------------------------------------------------------------------------

float TimelineProxy::GetRcpNumTicksPerBeat() const
{
    return mTimeline->GetRcpNumTicksPerBeat();
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

//----------------------------------------------------------------------------------------

AssetLib::IRuntimeAssetObjectProxy* TimelineProxy::GetDecoratedObject() const
{
    return mTimeline->GetRuntimeAssetObjectProxy();
}

}   // namespace Timeline
}   // namespace Pegasus


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif  // PEGASUS_ENABLE_PROXIES
