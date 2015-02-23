/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderProxy.cpp
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shaders

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"

Pegasus::Shader::ShaderProxy::ShaderProxy(Pegasus::Shader::ShaderStage * object)
: mObject(object)
{
}

Pegasus::Shader::ShaderType Pegasus::Shader::ShaderProxy::GetStageType() const
{
    return mObject->GetStageType(); 
}

const char * Pegasus::Shader::ShaderProxy::GetName() const
{
    return mObject->GetFileName();
}

void Pegasus::Shader::ShaderProxy::GetSource(const char ** outSrc, int& outSize) const
{
    return mObject->GetSource(outSrc, outSize);
}

void Pegasus::Shader::ShaderProxy::SetSource(const char * src, int size)
{
    mObject->SetSource(src, size);
}

void Pegasus::Shader::ShaderProxy::SaveSourceToFile()
{
    mObject->SaveSourceToFile();
}

void Pegasus::Shader::ShaderProxy::SetUserData(Pegasus::Graph::IGraphUserData * userData)
{
#if PEGASUS_USE_GRAPH_EVENTS
    mObject->SetGraphEventUserData(userData);
#endif
}

Pegasus::Graph::IGraphUserData * Pegasus::Shader::ShaderProxy::GetUserData() const
{
#if PEGASUS_USE_GRAPH_EVENTS
    return mObject->GetGraphEventUserData();
#else
    return nullptr;
#endif
}

void Pegasus::Shader::ShaderProxy::Compile()
{
    mObject->Compile();
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif

