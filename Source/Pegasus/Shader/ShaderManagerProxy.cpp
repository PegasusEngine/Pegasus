/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderManagerProxy.cpp
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shaders

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/ShaderManagerProxy.h"
#include "Pegasus/Shader/ProgramProxy.h"
#include "Pegasus/Shader/ShaderManager.h"

Pegasus::Shader::ShaderManagerProxy::ShaderManagerProxy(Pegasus::Shader::ShaderManager * object)
: mObject(object)
{
}

int Pegasus::Shader::ShaderManagerProxy::GetProgramCount() const
{
    return mObject->GetShaderTracker()->ProgramSize();
}

Pegasus::Shader::IProgramProxy * Pegasus::Shader::ShaderManagerProxy::GetProgram(int i)
{
    if (i >= 0 && i < GetProgramCount())
    {
        Pegasus::Shader::ProgramProxy * proxyWrapper = &mProgramProxyPool[i];
        proxyWrapper->Wrap(mObject->GetShaderTracker()->GetProgram(i));
        return proxyWrapper;
    }
    return nullptr;
}

int Pegasus::Shader::ShaderManagerProxy::GetShaderCount() const
{
    return mObject->GetShaderTracker()->ShaderSize();
}

Pegasus::Shader::IShaderProxy * Pegasus::Shader::ShaderManagerProxy::GetShader(int i)
{
    if (i >= 0 && i < GetShaderCount())
    {
        Pegasus::Shader::ShaderProxy * proxyWrapper = &mShaderProxyPool[i];
        proxyWrapper->Wrap(mObject->GetShaderTracker()->GetShaderStage(i));
        return proxyWrapper;
    }
    return nullptr;
}

void Pegasus::Shader::ShaderManagerProxy::RegisterEventListener(Pegasus::Shader::IEventListener * eventListener)
{
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    mObject->RegisterEventListener(eventListener);
#endif
}

void Pegasus::Shader::ShaderManagerProxy::UpdateAllPrograms()
{ 
    const Pegasus::Shader::ShaderTracker * tracker = mObject->GetShaderTracker();
    bool updated;
    for (int i = 0; i < tracker->ProgramSize(); ++i)
    {
        updated = false;
        tracker->GetProgram(i)->GetUpdatedData(updated);
    }
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
