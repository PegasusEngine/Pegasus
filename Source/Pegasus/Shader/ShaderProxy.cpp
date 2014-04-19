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

#include "Pegasus/Shader/ShaderProxy.h"
#include "Pegasus/Shader/ShaderStage.h"

Pegasus::Shader::ShaderProxy::ShaderProxy()
: mObject(nullptr)
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

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif

