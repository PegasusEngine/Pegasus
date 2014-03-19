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
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
