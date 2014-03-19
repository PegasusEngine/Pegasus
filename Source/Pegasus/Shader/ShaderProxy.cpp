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

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif

