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
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/ASTree.h"


using namespace Pegasus;

#define REGISTER_SHADER_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode);

Pegasus::Shader::ShaderManager::ShaderManager(Pegasus::Graph::NodeManager * nodeManager)
:   mNodeManager(nodeManager)
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
    REGISTER_SHADER_NODE(ShaderSource);
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram()
{
    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
    program->SetManager(this);
#if PEGASUS_USE_EVENTS 
    program->SetEventListener(mEventListener);
#endif

#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(program->GetProxy()), "ProgramLinkage", mEventListener);
#endif

    return program; 
}

Pegasus::Shader::ShaderSourceReturn Pegasus::Shader::ShaderManager::CreateShaderSrc()
{    
    Pegasus::Shader::ShaderSourceRef stage = mNodeManager->CreateNode("ShaderSource");
    #if PEGASUS_USE_EVENTS
    stage->SetEventListener(mEventListener);
    #endif

    #if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_INIT_USER_DATA(static_cast<Pegasus::Core::IBasicSourceProxy*>(stage->GetProxy()), "ShaderSource", mEventListener);
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

Pegasus::Shader::ShaderSourceRef Pegasus::Shader::ShaderManager::LoadShaderSrc(const char* filename)
{
    AssetLib::RuntimeAssetObjectRef obj = GetAssetLib()->LoadObject(filename);
    if (obj != nullptr && obj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_SHADER_SRC.mTypeGuid)
    {
        return obj;
    }
    return nullptr;
}

AssetLib::RuntimeAssetObjectRef Pegasus::Shader::ShaderManager::CreateRuntimeObject(const Pegasus::PegasusAssetTypeDesc* desc)
{
    if (desc->mTypeGuid == ASSET_TYPE_PROGRAM.mTypeGuid)
    {
        return CreateProgram();
    }
    else if (desc->mTypeGuid == ASSET_TYPE_SHADER_SRC.mTypeGuid)
    {
        return CreateShaderSrc();
    }
    
    return nullptr;
}

const Pegasus::PegasusAssetTypeDesc*const* Pegasus::Shader::ShaderManager::GetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gDescs[] = {
        &ASSET_TYPE_SHADER_SRC,
        &ASSET_TYPE_PROGRAM,
        nullptr
    };
    return gDescs;
}

