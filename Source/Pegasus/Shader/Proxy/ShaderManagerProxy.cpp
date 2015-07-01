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
: mObject(object), mOpenedShaders(Memory::GetGlobalAllocator()), mOpenedPrograms(Memory::GetGlobalAllocator())
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
        Pegasus::Shader::IProgramProxy * proxyWrapper = mObject->GetShaderTracker()->GetProgram(i)->GetProxy();
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
        Pegasus::Shader::IShaderProxy * proxyWrapper = mObject->GetShaderTracker()->GetShaderSource(i)->GetProxy();
        return proxyWrapper;
    }
    return nullptr;
}
 
void Pegasus::Shader::ShaderManagerProxy::RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener)
{
#if PEGASUS_USE_GRAPH_EVENTS
    mObject->RegisterEventListener(eventListener);
#endif
}

Pegasus::Shader::IShaderProxy* Pegasus::Shader::ShaderManagerProxy::OpenShader(AssetLib::IAssetProxy* asset)
{
    const char* ext = Utils::Strrchr(asset->GetPath(), '.');
    Pegasus::Shader::ShaderSourceRef shader = (ext != nullptr && !Utils::Strcmp(ext, ".h")) ?
         mObject->CreateHeader(static_cast<AssetLib::AssetProxy*>(asset)->GetObject()) :
         mObject->CreateShader(static_cast<AssetLib::AssetProxy*>(asset)->GetObject());
    for (int i = 0; i < mOpenedShaders.GetSize(); ++i)
    {
        Pegasus::Shader::ShaderSourceRef& shaderCandidate = mOpenedShaders[i];
        if (&(*shaderCandidate) == &(*shader))
        {
            //found the shader, dont add it to the list, just return it
            return shader->GetProxy();
        }
    }
    mOpenedShaders.PushEmpty() = shader; //add a reference
    return shader->GetProxy();
}

void Pegasus::Shader::ShaderManagerProxy::InternalOpenProgram(Pegasus::Shader::ProgramLinkageRef program)
{
    for (int i = 0; i < mOpenedPrograms.GetSize(); ++i)
    {
        Pegasus::Shader::ProgramLinkageRef& programCandidate = mOpenedPrograms[i];
        if (&(*program) == &(*programCandidate))
        {
            return;
        }
    }
    mOpenedPrograms.PushEmpty() = program; //add a reference
}

Pegasus::Shader::IProgramProxy* Pegasus::Shader::ShaderManagerProxy::OpenProgram(AssetLib::IAssetProxy* asset)
{
    Pegasus::Shader::ProgramLinkageRef program = mObject->CreateProgram(static_cast<AssetLib::AssetProxy*>(asset)->GetObject());
    InternalOpenProgram(program);
    return program->GetProxy();
}

Pegasus::Shader::IProgramProxy* Pegasus::Shader::ShaderManagerProxy::CreateNewProgram(const char* programName)
{
    Pegasus::Shader::ProgramLinkageRef program = mObject->CreateProgram(programName);
    InternalOpenProgram(program);
    return program->GetProxy();
    
}

void Pegasus::Shader::ShaderManagerProxy::CloseShader(Pegasus::Shader::IShaderProxy* shaderProxy)
{
    //try to find the shader
    for (int i = 0; i < mOpenedShaders.GetSize(); ++i)
    {
        Pegasus::Shader::ShaderSourceRef& shader = mOpenedShaders[i];
        if (&(*shader) == static_cast<Pegasus::Shader::ShaderProxy*>(shaderProxy)->GetObject())
        {
            shader = nullptr; //remove reference
            mOpenedShaders.Delete(i); //remove from list
        }
    }
}

void Pegasus::Shader::ShaderManagerProxy::CloseProgram(Pegasus::Shader::IProgramProxy* programProxy)
{
    //try to find the shader
    for (int i = 0; i < mOpenedPrograms.GetSize(); ++i)
    {
        Pegasus::Shader::ProgramLinkageRef& program = mOpenedPrograms[i];
        if (&(*program) == static_cast<ProgramProxy*>(programProxy)->GetObject())
        {
            program = nullptr; //remove reference
            mOpenedPrograms.Delete(i); //remove from list
        }
    }
}

bool Pegasus::Shader::ShaderManagerProxy::IsShader(const AssetLib::IAssetProxy* asset) const 
{
    const char* ext = Pegasus::Utils::Strrchr(asset->GetPath(), '.');
    return
          (ext != nullptr && !Utils::Strcmp(ext, ".h"))
         || mObject->DeriveShaderType(static_cast<const AssetLib::AssetProxy*>(asset)->GetObject()) != Pegasus::Shader::SHADER_STAGE_INVALID;
}

bool Pegasus::Shader::ShaderManagerProxy::IsProgram(const AssetLib::IAssetProxy* asset) const
{
    return mObject->IsProgram(static_cast<const AssetLib::AssetProxy*>(asset)->GetObject());
}

Pegasus::AssetLib::IAssetProxy* Pegasus::Shader::ShaderManagerProxy::GetShaderAsset(Pegasus::Shader::IShaderProxy* shader)
{
    Pegasus::Shader::ShaderSource* obj = static_cast<Pegasus::Shader::ShaderProxy*>(shader)->GetObject();
    if (obj->GetOwnerAsset() != nullptr)
    {
        return obj->GetOwnerAsset()->GetProxy();
    }

    return nullptr;

}

Pegasus::AssetLib::IAssetProxy* Pegasus::Shader::ShaderManagerProxy::GetProgramAsset(Pegasus::Shader::IProgramProxy* program)
{
    Pegasus::Shader::ProgramLinkage* obj = static_cast<Pegasus::Shader::ProgramProxy*>(program)->GetObject();
    if (obj->GetOwnerAsset() != nullptr)
    {
        return obj->GetOwnerAsset()->GetProxy();
    }

    return nullptr;
}

void Pegasus::Shader::ShaderManagerProxy::FlushShaderToAsset(Pegasus::Shader::IShaderProxy* shader)
{
    Pegasus::Shader::ShaderSourceRef obj = static_cast<Pegasus::Shader::ShaderProxy*>(shader)->GetObject();
    mObject->FlushShaderToAsset(obj);
}

void Pegasus::Shader::ShaderManagerProxy::FlushProgramToAsset(Pegasus::Shader::IProgramProxy* program)
{
    Pegasus::Shader::ProgramLinkageRef obj = static_cast<Pegasus::Shader::ProgramProxy*>(program)->GetObject();
    mObject->FlushProgramToAsset(obj);
}

void Pegasus::Shader::ShaderManagerProxy::BindProgramToAsset(Pegasus::Shader::IProgramProxy* programProxy, Pegasus::AssetLib::IAssetProxy* assetProxy)
{
    Pegasus::AssetLib::Asset* asset = nullptr;
    if (assetProxy != nullptr)
    {
        asset = static_cast<Pegasus::AssetLib::AssetProxy*>(assetProxy)->GetObject();
    }
    Pegasus::Shader::ProgramLinkage* program = static_cast<Pegasus::Shader::ProgramProxy*>(programProxy)->GetObject();
    
    if (program->GetOwnerAsset() != nullptr)
    {
        mObject->GetAssetLib()->UnbindAssetToRuntimeObject(program);
    }

    if (asset != nullptr)
    {
        mObject->GetAssetLib()->BindAssetToRuntimeObject(asset, program);
    }
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
