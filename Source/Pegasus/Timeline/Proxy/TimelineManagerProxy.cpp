/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineManagerProxy.cpp
//! \author	Karolyn Boulanger
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
    :   mTimelineManager(timelineManager)
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
    return static_cast<Core::ISourceCodeProxy*>(mTimelineManager->GetScriptTracker()->GetScriptById(id)->GetProxy());
}

void TimelineManagerProxy::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
    mTimelineManager->RegisterEventListener(eventListener);
}

}   // namespace Timeline
}   // namespace Pegasus


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif  // PEGASUS_ENABLE_PROXIES
