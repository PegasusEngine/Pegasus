/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ScriptProxy.cpp
//! \author	Kleber Garcia
//! \date	5th November 2014
//! \brief	Proxy class for interactions with editor
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Proxy/ScriptProxy.h"
#include "Pegasus/Graph/Shared/GraphEventDefs.h"
#include "Pegasus/Core/Io.h"

#include "Pegasus/Timeline/ScriptHelper.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;

ScriptProxy::ScriptProxy(ScriptHelper* helper) : mScriptHelper(helper) {}

ScriptProxy::~ScriptProxy() {}

const char * ScriptProxy::GetName() const
{
    return mScriptHelper->GetScriptName();
}

void ScriptProxy::GetSource(const char ** outSrc, int& outSize) const
{
    mScriptHelper->GetSource(outSrc, outSize);
}

void ScriptProxy::SetSource(const char * source, int sourceSize)
{
    mScriptHelper->SetSource(source, sourceSize);
}

void ScriptProxy::SaveSourceToFile()
{
    mScriptHelper->SaveScriptToFile();
}

void ScriptProxy::SetUserData(Pegasus::Graph::IGraphUserData * userData)
{
    mScriptHelper->SetGraphEventUserData(userData);
}

Graph::IGraphUserData * ScriptProxy::GetUserData() const
{
    return mScriptHelper->GetGraphEventUserData();
}

long long ScriptProxy::GetGuid() const
{
    return 0;
}
#endif
