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

#define REGISTER_SHADER_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode);

Pegasus::Shader::ShaderManager::ShaderManager(Pegasus::Graph::NodeManager * nodeManager, Pegasus::Shader::IShaderFactory * factory)
:   mNodeManager(nodeManager)
,   mFactory(factory)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mShaderTracker(Pegasus::Memory::GetGlobalAllocator())
#endif  // PEGASUS_ENABLE_PROXIES
#if PEGASUS_USE_GRAPH_EVENTS
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

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::CreateShader(AssetLib::Asset* asset)
{
    //has this asset been already built?
    if (asset->GetRuntimeData() != nullptr)
    {
        Pegasus::Shader::ShaderStageRef shaderStageRef = static_cast<Pegasus::Shader::ShaderStage*>(asset->GetRuntimeData());
        return shaderStageRef;
    }

    Pegasus::Shader::ShaderStageRef stage = mNodeManager->CreateNode("ShaderStage");

#if PEGASUS_USE_GRAPH_EVENTS
    stage->SetEventListener(mEventListener);
#endif
    stage->SetFactory(mFactory);

    if (stage->Read(asset))
    {
#if PEGASUS_ENABLE_PROXIES
        GRAPH_EVENT_INIT_USER_DATA(stage->GetProxy(), "ShaderStage", mEventListener);
        stage->SetShaderTracker(&mShaderTracker);
        stage->SetFullFilePath(asset->GetPath());
        mShaderTracker.InsertShader(&(*stage));
#endif
        return stage;
    }
    else
    {
        return nullptr;
    }
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram(const char* name)
{
    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
    program->SetManager(this);
#if PEGASUS_USE_GRAPH_EVENTS 
    program->SetEventListener(mEventListener);
#endif
    program->SetFactory(mFactory);
#if PEGASUS_ENABLE_PROXIES
    //if proxies make sure to set metadata correctly
    program->SetName(name);
    program->SetShaderTracker(&mShaderTracker);
    mShaderTracker.InsertProgram(&(*program));
#endif

#if PEGASUS_ENABLE_PROXIES
    GRAPH_EVENT_INIT_USER_DATA(program->GetProxy(), "ProgramLinkage", mEventListener);
#endif

    return program; 
}

bool Pegasus::Shader::ShaderManager::IsProgram(const AssetLib::Asset* asset) const
{
    return Pegasus::Shader::ProgramLinkage::IsProgram(asset);
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram(AssetLib::Asset* asset)
{
    if (asset->GetRuntimeData() != nullptr)
    {
        Pegasus::Shader::ProgramLinkageRef programRef = static_cast<Pegasus::Shader::ProgramLinkage*>(asset->GetRuntimeData());
        return programRef;
    }

    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
    program->SetManager(this);
#if PEGASUS_USE_GRAPH_EVENTS 
    program->SetEventListener(mEventListener);
#endif
    program->SetFactory(mFactory);

    if (program->Read(asset))
    {

    #if PEGASUS_ENABLE_PROXIES
        //if proxies make sure to set metadata correctly
        program->SetShaderTracker(&mShaderTracker);
        mShaderTracker.InsertProgram(&(*program));
        GRAPH_EVENT_INIT_USER_DATA(program->GetProxy(), "ProgramLinkage", mEventListener);
    #endif
    
        return program; 
    }
    else
    {
        return nullptr;
    }
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::LoadProgram(const char* filename)
{
    AssetLib::Asset* asset = nullptr;
    Io::IoError err = mAssetLib->LoadAsset(filename, &asset);
    if (err == Io::ERR_NONE && asset != nullptr)
    {
        if (!IsProgram(asset))
        {
            PG_LOG('ERR_', "Cannot build a program with an asset %s'", asset->GetPath());
            return nullptr;
        }

        Pegasus::Shader::ProgramLinkageRef program = CreateProgram(asset);
        if (program != nullptr)
        {
            return program;
        }
        else
        {
            mAssetLib->DestroyAsset(asset);
        }
    }
    else
    {
        PG_LOG('ERR_', "Error loading program %s", filename);
    }
    
    return nullptr;
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::LoadShader(const char* filename)
{    
    AssetLib::Asset* asset = nullptr;
    Io::IoError err = mAssetLib->LoadAsset(filename, &asset);
    if (err == Io::ERR_NONE && asset != nullptr)
    {
        Pegasus::Shader::ShaderStageRef shader = CreateShader(asset);
        if (shader != nullptr)
        {
            return shader;
        }
        else
        {
            mAssetLib->DestroyAsset(asset);
        }
    }
    else
    {
        PG_LOG('ERR_', "Error loading shader %s", filename);
    }
    
    return nullptr;
}

Pegasus::Shader::ShaderSourceReturn Pegasus::Shader::ShaderManager::LoadHeader(const char* filename)
{    
    AssetLib::Asset* asset = nullptr;
    Io::IoError err = mAssetLib->LoadAsset(filename, &asset);
    if (err == Io::ERR_NONE && asset != nullptr)
    {
        Pegasus::Shader::ShaderSourceRef stage = CreateHeader(asset);
        if (stage == nullptr)
        {
            mAssetLib->DestroyAsset(asset);
        }

        return stage;
    }
    else
    {
        PG_LOG('ERR_', "Error loading shader %s", filename);
    }
    
    return nullptr;
}

Pegasus::Shader::ShaderSourceReturn Pegasus::Shader::ShaderManager::CreateHeader(AssetLib::Asset* asset)
{    
    //has this asset been already built?
    if (asset->GetRuntimeData() != nullptr)
    {
        //TODO: dangerous cast! we could be including a texture file!
        Pegasus::Shader::ShaderSourceRef shaderSource = static_cast<Pegasus::Shader::ShaderSource*>(asset->GetRuntimeData());
        return shaderSource;
    }
    
    Pegasus::Shader::ShaderSourceRef stage = mNodeManager->CreateNode("ShaderSource");
    
    if (stage->Read(asset))
    {
        #if PEGASUS_USE_GRAPH_EVENTS
            stage->SetEventListener(mEventListener);
        #endif

        #if PEGASUS_ENABLE_PROXIES
            //TODO: kill this
            stage->SetFullFilePath(asset->GetPath());
        #endif
        
        #if PEGASUS_ENABLE_PROXIES
            GRAPH_EVENT_INIT_USER_DATA(stage->GetProxy(), "ShaderStage", mEventListener);
        #endif

        return stage;
    }
    else
    {
        return nullptr;
    }
}
