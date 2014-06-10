/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ProgramProxy.cpp
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shaders

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/ShaderProxy.h"
#include "Pegasus/Shader/ProgramProxy.h"
#include "Pegasus/Shader/ProgramLinkage.h"

Pegasus::Shader::ProgramProxy::ProgramProxy(Pegasus::Shader::ProgramLinkage * object)
: mObject(object)
{
}

int Pegasus::Shader::ProgramProxy::GetShaderCount() const
{
    return mObject->GetNumInputs();
}

Pegasus::Shader::IShaderProxy * Pegasus::Shader::ProgramProxy::GetShader(unsigned i)
{
    if (i < mObject->GetNumInputs() && i < Pegasus::Shader::SHADER_STAGES_COUNT)
    {
        Pegasus::Shader::ShaderStageRef shaderInput = mObject->GetInput(i);
        Pegasus::Shader::ShaderStage * shaderInputUnsafe = &(*shaderInput);
        return shaderInputUnsafe->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

const char * Pegasus::Shader::ProgramProxy::GetName() const
{
    return mObject->GetName();
}

void Pegasus::Shader::ProgramProxy::SetUserData(Pegasus::Graph::IGraphUserData * userData)
{
#if PEGASUS_USE_GRAPH_EVENTS
    mObject->SetGraphEventUserData(userData);
#endif
}

Pegasus::Graph::IGraphUserData * Pegasus::Shader::ProgramProxy::GetUserData() const
{
#if PEGASUS_USE_GRAPH_EVENTS
    return mObject->GetGraphEventUserData();
#else
    return nullptr;
#endif
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
