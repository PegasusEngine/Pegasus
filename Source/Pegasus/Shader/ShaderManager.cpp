/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderManager.cpp
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Main shader interface

#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/IShaderFactory.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/ASTree.h"


using namespace Pegasus;

static bool IsShader(const PegasusAssetTypeDesc* desc)
{
    return (
        desc->mTypeGuid == ASSET_TYPE_VS_SHADER.mTypeGuid
    ||  desc->mTypeGuid == ASSET_TYPE_PS_SHADER.mTypeGuid
    ||  desc->mTypeGuid == ASSET_TYPE_GS_SHADER.mTypeGuid
    ||  desc->mTypeGuid == ASSET_TYPE_TCS_SHADER.mTypeGuid
    ||  desc->mTypeGuid == ASSET_TYPE_TES_SHADER.mTypeGuid
    ||  desc->mTypeGuid == ASSET_TYPE_CS_SHADER.mTypeGuid
    );
}

#define REGISTER_SHADER_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode);

Pegasus::Shader::ShaderManager::ShaderManager(Pegasus::Graph::NodeManager * nodeManager, Pegasus::Shader::IShaderFactory * factory)
:   mNodeManager(nodeManager)
,   mFactory(factory)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif  // PEGASUS_ENABLE_PROXIES
#if PEGASUS_USE_EVENTS
,   mEventListener(nullptr)
#endif
{
    if (nodeManager != nullptr)
    {
        RegisterAllNodes();
    }
    else
    {
        PG_FAILSTR("Invalid node manager given to ShaderManager");
    }
}

Pegasus::Shader::ShaderManager::~ShaderManager()
{
}

void Pegasus::Shader::ShaderManager::RegisterAllNodes()
{
    PG_ASSERTSTR(mNodeManager != nullptr, "Enable to register the shader nodes because the texture manager is not linke to the node manager");

    REGISTER_SHADER_NODE(ProgramLinkage);
    REGISTER_SHADER_NODE(ShaderStage);
    REGISTER_SHADER_NODE(ShaderSource);
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::CreateShader()
{

    Pegasus::Shader::ShaderStageRef stage = mNodeManager->CreateNode("ShaderStage");

#if PEGASUS_USE_EVENTS
    stage->SetEventListener(mEventListener);
#endif
    stage->SetFactory(mFactory);

#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(stage->GetProxy()), "ShaderStage", mEventListener);
#endif
    return stage;
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram()
{
    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
    program->SetManager(this);
#if PEGASUS_USE_EVENTS 
    program->SetEventListener(mEventListener);
#endif
    program->SetFactory(mFactory);

#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(program->GetProxy()), "ProgramLinkage", mEventListener);
#endif

    return program; 
}

Pegasus::Shader::ShaderSourceReturn Pegasus::Shader::ShaderManager::CreateHeader()
{    
    Pegasus::Shader::ShaderSourceRef stage = mNodeManager->CreateNode("ShaderSource");
    #if PEGASUS_USE_EVENTS
    stage->SetEventListener(mEventListener);
    #endif

    #if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(stage->GetProxy()), "ShaderStage", mEventListener);
    #endif
    return stage;
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::LoadProgram(const char* filename)
{
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(filename);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_PROGRAM.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::LoadShader(const char* filename)
{    
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(filename);
    if (obj != nullptr && IsShader(obj->GetOwnerAsset()->GetTypeDesc()))
    {
        return obj;
    }
    return nullptr;
}

Pegasus::Shader::ShaderSourceReturn Pegasus::Shader::ShaderManager::LoadHeader(const char* filename)
{    
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(filename);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_H_SHADER.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

AssetLib::RuntimeAssetObjectRef Pegasus::Shader::ShaderManager::CreateRuntimeObject(const Pegasus::PegasusAssetTypeDesc* desc)
{
    if (IsShader(desc))
    {
        Pegasus::Shader::ShaderStageRef shaderStage = CreateShader();
        shaderStage->SetStageType(Pegasus::Shader::ShaderStage::DeriveShaderTypeFromExt(desc->mExtension));
        return shaderStage;
    }
    else if (desc->mTypeGuid == ASSET_TYPE_PROGRAM.mTypeGuid)
    {
        return CreateProgram();
    }
    else if (desc->mTypeGuid == ASSET_TYPE_H_SHADER.mTypeGuid)
    {
        return CreateHeader();
    }
    
    return nullptr;
}

const Pegasus::PegasusAssetTypeDesc*const* Pegasus::Shader::ShaderManager::GetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gDescs[] = {
         &ASSET_TYPE_H_SHADER
       , &ASSET_TYPE_VS_SHADER
       , &ASSET_TYPE_PS_SHADER
       , &ASSET_TYPE_GS_SHADER
       , &ASSET_TYPE_TCS_SHADER
       , &ASSET_TYPE_TES_SHADER
       , &ASSET_TYPE_CS_SHADER
       , &ASSET_TYPE_PROGRAM
       , nullptr
    };
    return gDescs;
}

