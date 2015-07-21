/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineManagerProxy.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline

//! \todo Why do we need this in Rel-Debug? TimelineManagerProxy should not even be compiled in REL mode
#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/TimelineManagerProxy.h"
#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/Timeline/Proxy/LaneProxy.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;

namespace Pegasus {
namespace Timeline {


TimelineManagerProxy::TimelineManagerProxy(TimelineManager * timelineManager)
    :   mTimelineManager(timelineManager), mOpenedScripts(Pegasus::Memory::GetGlobalAllocator())
{
    PG_ASSERTSTR(timelineManager != nullptr, "Trying to create a timeline proxy from an invalid timeline object");
}

//----------------------------------------------------------------------------------------

TimelineManagerProxy::~TimelineManagerProxy()
{
}

//----------------------------------------------------------------------------------------

unsigned int TimelineManagerProxy::GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                                    char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const
{
    return mTimelineManager->GetRegisteredBlockNames(classNames, editorStrings);
}

ITimelineProxy* TimelineManagerProxy::GetCurrentTimeline() const
{
    Timeline* t = mTimelineManager->GetCurrentTimeline();
    if (t == nullptr)
    {
        return nullptr;
    }
    else
    {
        return t->GetProxy();
    }
}

void TimelineManagerProxy::Update()
{
    mTimelineManager->Update();
}

int TimelineManagerProxy::GetSourceCount() const
{
    return mTimelineManager->GetScriptTracker()->GetScriptCount();
}

Core::ISourceCodeProxy* TimelineManagerProxy::GetSource(int id)
{
    return mTimelineManager->GetScriptTracker()->GetScriptById(id)->GetProxy();
}

void TimelineManagerProxy::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
    mTimelineManager->RegisterEventListener(eventListener);
}

Core::ISourceCodeProxy* TimelineManagerProxy::OpenScript(const char* path)
{
    TimelineSourceRef script = mTimelineManager->LoadScript(path);
    if (script != nullptr)
    {
        if (FindOpenedScript(script) == -1)
        {
            *(new (&mOpenedScripts.PushEmpty()) TimelineSourceRef)  = script;
        }
        return script->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

Core::ISourceCodeProxy* TimelineManagerProxy::OpenScript(AssetLib::IAssetProxy* asset)
{
    AssetLib::AssetProxy* assetProxy = static_cast<AssetLib::AssetProxy*>(asset);
    const char* ext = Utils::Strrchr(asset->GetPath(), '.');

    TimelineSourceRef script = Utils::Strcmp(ext, ".bsh") ? 
            mTimelineManager->CreateScript(assetProxy->GetObject()) :
            mTimelineManager->CreateHeader(assetProxy->GetObject()) ;

    if (script != nullptr)
    {
        if (FindOpenedScript(script) == -1)
        {
            *(new (&mOpenedScripts.PushEmpty()) TimelineSourceRef) = script;
        }
        return script->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

void TimelineManagerProxy::CloseScript(Core::ISourceCodeProxy* script)
{
    TimelineSourceRef timelineScript = static_cast<TimelineScriptProxy*>(script)->GetObject();
    int index = FindOpenedScript(timelineScript);
    if (index != -1)
    {
        mOpenedScripts[index] = nullptr; //decrease the reference to the current script, a potential destructor call
        mOpenedScripts.Delete(index);
    }
}

int TimelineManagerProxy::FindOpenedScript(TimelineSourceIn script)
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

bool TimelineManagerProxy::IsTimelineScript(const AssetLib::IAssetProxy* asset) const
{
    return mTimelineManager->IsTimelineScript(static_cast<const AssetLib::AssetProxy*>(asset)->GetObject());
}


}   // namespace Timeline
}   // namespace Pegasus


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif  // PEGASUS_ENABLE_PROXIES
