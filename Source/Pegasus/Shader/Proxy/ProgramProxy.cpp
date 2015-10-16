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

#include "Pegasus/Shader/Proxy/ShaderProxy.h"
#include "Pegasus/Shader/Proxy/ProgramProxy.h"
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
        return static_cast<Pegasus::Shader::IShaderProxy *>(shaderInputUnsafe->GetProxy());
    }
    else
    {
        return nullptr;
    }
}

void Pegasus::Shader::ProgramProxy::RemoveShader(unsigned i)
{
    Pegasus::Shader::ShaderStageRef stage = mObject->FindShaderStageInput(i);
    if (stage != nullptr)
    {
        mObject->RemoveShaderStage(stage->GetStageType());
    }
}

void Pegasus::Shader::ProgramProxy::SetShader(Pegasus::Shader::IShaderProxy* shader)
{
    Pegasus::Shader::ShaderProxy* shaderProxy = static_cast<Pegasus::Shader::ShaderProxy*>(shader);
    if (shaderProxy->GetObject()->GetStageType() != Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        Pegasus::Shader::ShaderStageRef shaderRef = static_cast<Pegasus::Shader::ShaderStage*>(shaderProxy->GetObject());
        mObject->SetShaderStage(shaderRef);
    }
    else
    {
        PG_LOG('ERR_', "Shader does not have a correct state %s.", shader->GetName());
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

Pegasus::AssetLib::IRuntimeAssetObjectProxy* Pegasus::Shader::ProgramProxy::GetDecoratedObject() const
{
    return mObject->GetRuntimeAssetObjectProxy();
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
