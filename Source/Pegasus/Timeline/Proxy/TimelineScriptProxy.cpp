/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScriptProxy.cpp
//! \author	Kleber Garcia
//! \date	5th November 2014
//! \brief	Proxy class for interactions with editor
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#include "Pegasus/Graph/Shared/GraphEventDefs.h"
#include "Pegasus/Core/Io.h"

#include "Pegasus/Timeline/TimelineScript.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

TimelineScriptProxy::TimelineScriptProxy(TimelineScript* helper) : mTimelineScript(helper) {}

TimelineScriptProxy::~TimelineScriptProxy() {}

const char * TimelineScriptProxy::GetName() const
{
    return mTimelineScript->GetScriptName();
}

void TimelineScriptProxy::GetSource(const char ** outSrc, int& outSize) const
{
    mTimelineScript->GetSource(outSrc, outSize);
}

void TimelineScriptProxy::SetSource(const char * source, int sourceSize)
{
    mTimelineScript->SetSource(source, sourceSize);
}

void TimelineScriptProxy::SetUserData(Pegasus::Graph::IGraphUserData * userData)
{
    mTimelineScript->SetGraphEventUserData(userData);
}

Graph::IGraphUserData * TimelineScriptProxy::GetUserData() const
{
    return mTimelineScript->GetGraphEventUserData();
}

long long TimelineScriptProxy::GetGuid() const
{
    return 0;
}

void TimelineScriptProxy::Compile()
{ 
    mTimelineScript->CheckAndUpdateCompilationState();
}
#endif
