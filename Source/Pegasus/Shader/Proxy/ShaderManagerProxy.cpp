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

#include "Pegasus/Shader/Proxy/ShaderManagerProxy.h"
#include "Pegasus/Shader/Proxy/ProgramProxy.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/Utils/String.h"

Pegasus::Shader::ShaderManagerProxy::ShaderManagerProxy(Pegasus::Shader::ShaderManager * object)
: mObject(object)
{
}

void Pegasus::Shader::ShaderManagerProxy::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
#if PEGASUS_USE_EVENTS
    mObject->RegisterEventListener(eventListener);
#endif
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
