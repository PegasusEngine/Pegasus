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

#include <Pegasus/Shader/Proxy/ProgramProxy.h>
#include <Pegasus/Shader/Proxy/ShaderProxy.h>
#include <Pegasus/Shader/ProgramLinkage.h>
#include <Pegasus/Shader/ShaderSource.h>

Pegasus::Shader::ProgramProxy::ProgramProxy(Pegasus::Shader::ProgramLinkage * object)
: mObject(object)
{
}

const char * Pegasus::Shader::ProgramProxy::GetName() const
{
    return mObject->GetName();
}

void Pegasus::Shader::ProgramProxy::SetSourceCode(Pegasus::Core::ISourceCodeProxy* srcCodeProxy)
{
	ShaderSourceRef shaderSrcRef;
	if (srcCodeProxy != nullptr)
	{
		ShaderProxy* shaderProxy = static_cast<ShaderProxy*>(srcCodeProxy);
		shaderSrcRef = shaderProxy->GetObject();
	}    
    mObject->SetSourceCode(shaderSrcRef);
}

Pegasus::Core::ISourceCodeProxy* Pegasus::Shader::ProgramProxy::GetSourceCode() const
{
    auto sourceCodeObj = mObject->GetSourceCode();
    return sourceCodeObj == nullptr ? nullptr : (ShaderProxy*)sourceCodeObj->GetProxy();
}

void Pegasus::Shader::ProgramProxy::SetUserData(Pegasus::Core::IEventUserData  * userData)
{
#if PEGASUS_USE_EVENTS
    mObject->SetEventUserData(userData);
#endif
}

Pegasus::Core::IEventUserData * Pegasus::Shader::ProgramProxy::GetUserData() const
{
#if PEGASUS_USE_EVENTS
    return mObject->GetEventUserData();
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
