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
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/PegasusAssetTypes.h"



using namespace Pegasus;

namespace Pegasus {
namespace Timeline {


TimelineProxy::TimelineProxy(Timeline * timeline)
    :   mTimeline(timeline),
        mPropertyGridDecorator(timeline->GetScriptRunner(), timeline->GetPropertyGrid()->GetPropertyGridProxy())
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

IBlockProxy* TimelineProxy::CreateBlock(const char* className)
{
    Block* b = mTimeline->CreateBlock(className);

    if (b != nullptr)
    {
        return b->GetProxy();
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------

int TimelineProxy::DeleteBlock(unsigned int blockGuid)
{
    return mTimeline->DeleteBlock(blockGuid);
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

//----------------------------------------------------------------------------------------

IBlockProxy* TimelineProxy::FindBlockByGuid(unsigned blockGuid)
{
    unsigned int numOfLanes = GetNumLanes();
    for (unsigned l = 0; l < numOfLanes; ++l)
    {
        ILaneProxy* lane = GetLane(l);
        IBlockProxy* blocks[LANE_MAX_NUM_BLOCKS]; 
        unsigned numOfBlocks = lane->GetBlocks(blocks);
        for (unsigned b = 0; b < numOfBlocks; ++b)
        {
            IBlockProxy* blockProxy = blocks[b];
            if (blockProxy->GetGuid() == blockGuid)
            {
                return blockProxy;
            }
        }
    }
    
    return nullptr;
}

//----------------------------------------------------------------------------------------

Core::ISourceCodeProxy* TimelineProxy::GetScript() const
{
    TimelineScript* helper = mTimeline->GetScript();
    if (helper != nullptr)
    {
        return static_cast<Core::ISourceCodeProxy*>(helper->GetProxy());
    }

    return nullptr;
} 

//----------------------------------------------------------------------------------------

void TimelineProxy::ClearScript()
{
    mTimeline->ShutdownScript();
}

//----------------------------------------------------------------------------------------

void TimelineProxy::AttachScript(Core::ISourceCodeProxy* code)
{
    PG_ASSERT(code->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid);
    if (code->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
    {
        Pegasus::Timeline::TimelineScriptProxy* scriptProxy = static_cast<Pegasus::Timeline::TimelineScriptProxy*>(code);
        Pegasus::Timeline::TimelineScriptRef timelineScript = static_cast<Pegasus::Timeline::TimelineScript*>(scriptProxy->GetObject());
        mTimeline->AttachScript(timelineScript); 
    }
}

//----------------------------------------------------------------------------------------

void TimelineProxy::LoadMusic(const char* musicFileName)
{
    mTimeline->LoadMusic(musicFileName);
}

//----------------------------------------------------------------------------------------

void TimelineProxy::UnloadMusic()
{
    mTimeline->UnloadMusic();
}

//----------------------------------------------------------------------------------------

void TimelineProxy::DebugEnableSound(bool enableSound)
{
    mTimeline->DebugEnableSound(enableSound);
}

}   // namespace Timeline
}   // namespace Pegasus


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif  // PEGASUS_ENABLE_PROXIES
