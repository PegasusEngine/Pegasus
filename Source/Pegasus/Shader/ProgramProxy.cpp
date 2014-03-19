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
        mShaderProxyPool[i].Wrap(shaderInputUnsafe);        
        return &mShaderProxyPool[i];
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

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
