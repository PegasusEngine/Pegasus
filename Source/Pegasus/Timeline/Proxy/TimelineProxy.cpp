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

#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Memory/MemoryManager.h"

using namespace Pegasus;

namespace Pegasus {
namespace Timeline {


TimelineProxy::TimelineProxy(Timeline * timeline)
    :   mTimeline(timeline), mOpenedScripts(Pegasus::Memory::GetGlobalAllocator())
{
    PG_ASSERTSTR(timeline != nullptr, "Trying to create a timeline proxy from an invalid timeline object");
}

//----------------------------------------------------------------------------------------

TimelineProxy::~TimelineProxy()
{
}

//----------------------------------------------------------------------------------------

unsigned int TimelineProxy::GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                                    char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const
{
    return mTimeline->GetRegisteredBlockNames(classNames, editorStrings);
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

int TimelineProxy::GetSourceCount() const
{
    return mTimeline->GetScriptTracker()->GetScriptCount();
}

Core::ISourceCodeProxy* TimelineProxy::GetSource(int id)
{
    return mTimeline->GetScriptTracker()->GetScriptById(id)->GetProxy();
}

void TimelineProxy::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
    mTimeline->RegisterEventListener(eventListener);
}

Core::ISourceCodeProxy* TimelineProxy::OpenScript(const char* path)
{
    TimelineScriptRef script = mTimeline->LoadScript(path);
    if (script != nullptr)
    {
        if (FindOpenedScript(script) == -1)
        {
            *(new (&mOpenedScripts.PushEmpty()) TimelineScriptRef)  = script;
        }
        return script->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

Core::ISourceCodeProxy* TimelineProxy::OpenScript(AssetLib::IAssetProxy* asset)
{
    AssetLib::AssetProxy* assetProxy = static_cast<AssetLib::AssetProxy*>(asset);
    TimelineScriptRef script = mTimeline->CreateScript(assetProxy->GetObject());

    if (script != nullptr)
    {
        if (FindOpenedScript(script) == -1)
        {
            *(new (&mOpenedScripts.PushEmpty()) TimelineScriptRef) = script;
        }
        return script->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

void TimelineProxy::CloseScript(Core::ISourceCodeProxy* script)
{
    TimelineScriptRef timelineScript = static_cast<TimelineScriptProxy*>(script)->GetObject();
    int index = FindOpenedScript(timelineScript);
    if (index != -1)
    {
        mOpenedScripts[index] = nullptr; //decrease the reference to the current script, a potential destructor call
        mOpenedScripts.Delete(index);
    }
}

int TimelineProxy::FindOpenedScript(TimelineScriptIn script)
{
    for (int i = 0; i < mOpenedScripts.GetSize(); ++i)
    {
        if (&(*script) == &(*mOpenedScripts[i]))
        {
            return i;
        }
    }
    
    return -1;
}

bool TimelineProxy::IsTimelineScript(const AssetLib::IAssetProxy* asset) const
{
    return mTimeline->IsTimelineScript(static_cast<const AssetLib::AssetProxy*>(asset)->GetObject());
}


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
