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
#include "Pegasus/Core/Shared/EventDefs.h"
#include "Pegasus/Core/Io.h"

#include "Pegasus/Timeline/TimelineSource.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

TimelineScriptProxy::TimelineScriptProxy(TimelineSource* helper) : mTimelineSource(helper) {}

TimelineScriptProxy::~TimelineScriptProxy() {}

const char * TimelineScriptProxy::GetName() const
{
    return mTimelineSource->GetDisplayName();
}

void TimelineScriptProxy::GetSource(const char ** outSrc, int& outSize) const
{
    mTimelineSource->GetSource(outSrc, outSize);
}

void TimelineScriptProxy::SetSource(const char * source, int sourceSize)
{
    mTimelineSource->SetSource(source, sourceSize);
}

void TimelineScriptProxy::SetUserData(Pegasus::Core::IEventUserData * userData)
{
    mTimelineSource->SetEventUserData(userData);
}

Core::IEventUserData * TimelineScriptProxy::GetUserData() const
{
    return mTimelineSource->GetEventUserData();
}

long long TimelineScriptProxy::GetGuid() const
{
    return 0;
}

void TimelineScriptProxy::Compile()
{ 
    mTimelineSource->Compile();
}

AssetLib::IRuntimeAssetObjectProxy* TimelineScriptProxy::GetDecoratedObject() const
{
    return mTimelineSource->GetRuntimeAssetObjectProxy();
}
#endif
