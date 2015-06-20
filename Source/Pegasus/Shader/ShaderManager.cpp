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

Pegasus::Shader::ShaderType Pegasus::Shader::ShaderManager::DeriveShaderType(const AssetLib::Asset* asset) const
{
    if (asset->GetFormat() != AssetLib::Asset::FMT_RAW)
    {
        return Pegasus::Shader::SHADER_STAGE_INVALID;
    }

    const char * extension = Pegasus::Utils::Strrchr(asset->GetPath(), '.');
    Pegasus::Shader::ShaderType targetStage = Pegasus::Shader::SHADER_STAGE_INVALID;
    if (extension != nullptr)
    {
        return DeriveShaderType(extension);
    }

    return Pegasus::Shader::SHADER_STAGE_INVALID;
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::CreateShader(AssetLib::Asset* asset)
{
    Pegasus::Shader::ShaderType targetStage = DeriveShaderType(asset); 
    if (targetStage == Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        PG_LOG('ERR_', "Invalid shader extension");
        return nullptr;
    }

    //has this asset been already built?
    if (asset->GetRuntimeData() != nullptr)
    {
        Pegasus::Shader::ShaderStageRef shaderStageRef = static_cast<Pegasus::Shader::ShaderStage*>(asset->GetRuntimeData());
        return shaderStageRef;
    }

    Pegasus::Shader::ShaderStageRef stage = mNodeManager->CreateNode("ShaderStage");

    if (targetStage >= 0 && targetStage < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT))
    {
        Io::FileBuffer* fb = asset->Raw();
        stage->SetSource(targetStage, fb->GetBuffer(), fb->GetFileSize());
#if PEGASUS_ENABLE_PROXIES
        stage->SetShaderTracker(&mShaderTracker);
        mShaderTracker.InsertShader(&(*stage));
#endif
    }


#if PEGASUS_USE_GRAPH_EVENTS
    stage->SetEventListener(mEventListener);
#endif
    stage->SetFactory(mFactory);

#if PEGASUS_ENABLE_PROXIES
    //TODO: kill this
    stage->SetFullFilePath(asset->GetPath());
#endif
    
    
#if PEGASUS_ENABLE_PROXIES
    GRAPH_EVENT_INIT_USER_DATA(stage->GetProxy(), "ShaderStage", mEventListener);
#endif

    mAssetLib->BindAssetToRuntimeObject(asset, &(*stage));

    return stage;
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram(const char* name)
{
    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
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
    //parse asset
    if (asset->GetFormat() != AssetLib::Asset::FMT_STRUCTURED)
    {
        return false;
    }

    const AssetLib::Object* root = asset->Root();
    if (!root) return false;

    int typeId = root->FindString("type");

    if (typeId == -1)
    {
        return false;
    }

    const char* typeName = root->GetString(typeId);
    if (Utils::Stricmp(typeName, "program"))
    {
        return false;
    }

    return true;
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram(AssetLib::Asset* asset)
{
    if (asset->GetRuntimeData() != nullptr)
    {
        Pegasus::Shader::ProgramLinkageRef programRef = static_cast<Pegasus::Shader::ProgramLinkage*>(asset->GetRuntimeData());
        return programRef;
    }

    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
#if PEGASUS_USE_GRAPH_EVENTS 
    program->SetEventListener(mEventListener);
#endif
    program->SetFactory(mFactory);

    AssetLib::Object* root = asset->Root();
    int nameId = root->FindString("name");
    int shaders = root->FindArray("shaders");

#if PEGASUS_ENABLE_PROXIES
    //if proxies make sure to set metadata correctly
    if (nameId != -1)
    {
        program->SetName(root->GetString(nameId));
    }
    program->SetShaderTracker(&mShaderTracker);
    mShaderTracker.InsertProgram(&(*program));
#endif

    if (shaders != -1)
    {
        AssetLib::Array* shaderArr = root->GetArray(shaders);
        if (shaderArr->GetType() == AssetLib::Array::AS_TYPE_STRING)
        {
            for (int i = 0; i < shaderArr->GetSize(); ++i)
            {
                const AssetLib::Array::Element& element = shaderArr->GetElement(i);
                Pegasus::Shader::ShaderStageRef shaderStage = LoadShader(element.s);
                if (shaderStage != nullptr)
                {
                    program->SetShaderStage(shaderStage);
                }
            }
        }
        else if (shaderArr->GetSize() != 0) //forgivable error if empty list
        {
            PG_LOG('ERR_', "Invalid list of shaders");
            return nullptr;
        }
    }
    mAssetLib->BindAssetToRuntimeObject(asset, &(*program));

#if PEGASUS_ENABLE_PROXIES
    GRAPH_EVENT_INIT_USER_DATA(program->GetProxy(), "ProgramLinkage", mEventListener);
#endif

    return program; 
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
        if (asset->GetFormat() != Pegasus::AssetLib::Asset::FMT_RAW)
        {
            PG_LOG('ERR_', "Invalid shader extension");
            mAssetLib->DestroyAsset(asset);
            return nullptr;
        }
    
        //has this asset been already built?
        if (asset->GetRuntimeData() != nullptr)
        {
            //TODO: dangerous cast! we could be including a texture file!
            Pegasus::Shader::ShaderSourceRef shaderSource = static_cast<Pegasus::Shader::ShaderSource*>(asset->GetRuntimeData());
            return shaderSource;
        }
    
        Pegasus::Shader::ShaderSourceRef stage = mNodeManager->CreateNode("ShaderSource");
    
        Io::FileBuffer* fb = asset->Raw();
        stage->SetSource(fb->GetBuffer(), fb->GetFileSize());
    
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

        mAssetLib->BindAssetToRuntimeObject(asset, &(*stage));

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
    if (asset->GetFormat() != Pegasus::AssetLib::Asset::FMT_RAW)
    {
        PG_LOG('ERR_', "Invalid shader extension");
        mAssetLib->DestroyAsset(asset);
        return nullptr;
    }
    
    //has this asset been already built?
    if (asset->GetRuntimeData() != nullptr)
    {
        //TODO: dangerous cast! we could be including a texture file!
        Pegasus::Shader::ShaderSourceRef shaderSource = static_cast<Pegasus::Shader::ShaderSource*>(asset->GetRuntimeData());
        return shaderSource;
    }
    
    Pegasus::Shader::ShaderSourceRef stage = mNodeManager->CreateNode("ShaderSource");
    
    Io::FileBuffer* fb = asset->Raw();
    stage->SetSource(fb->GetBuffer(), fb->GetFileSize());
    
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

    mAssetLib->BindAssetToRuntimeObject(asset, &(*stage));

    return stage;
}

Pegasus::Shader::ShaderType Pegasus::Shader::ShaderManager::DeriveShaderType(const char* extension) const
{
    int extSz = Utils::Strlen(extension) - 1;
    const char* extSanitized = extension + 1;
    if (extSz > 0)
    {
        for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
        {
            if (!Pegasus::Utils::Stricmp(extSanitized, Pegasus::Shader::gShaderExtensions[i]))
            {
                return static_cast<Pegasus::Shader::ShaderType>(i);
            }
        }
    }

    return Pegasus::Shader::SHADER_STAGE_INVALID;
}

void Pegasus::Shader::ShaderManager::FlushShaderToAsset(Pegasus::Shader::ShaderSourceIn shaderStage)
{
    AssetLib::Asset* asset = shaderStage->GetOwnerAsset();
    const char* src = nullptr;
    int srcLen = 0;
    shaderStage->GetSource(&src, srcLen);
    Io::FileBuffer* fb = asset->Raw();
    if (srcLen > fb->GetBufferSize())
    {
        Pegasus::Alloc::IAllocator* alloc = fb->GetAllocator();
        fb->DestroyBuffer();
        fb->OwnBuffer(alloc, PG_NEW_ARRAY(alloc, -1, "", Alloc::PG_MEM_TEMP, char, srcLen), srcLen);
    }
    fb->SetFileSize(srcLen);
    Utils::Memcpy(fb->GetBuffer(), src, srcLen);
}

void Pegasus::Shader::ShaderManager::FlushProgramToAsset(Pegasus::Shader::ProgramLinkageInOut program)
{
    AssetLib::Asset* asset = program->GetOwnerAsset();
    PG_ASSERTSTR(asset != nullptr, "Can't flush this program because there is no asset bound to it");
    
    asset->Clear(); //brand  new asset
    AssetLib::Object* root = asset->NewObject();
    root->AddString("type", "program");
    asset->SetRootObject(root);

    AssetLib::Array* shaderArr = asset->NewArray();

#if PEGASUS_ENABLE_PROXIES
    root->AddString("name", program->GetName());
#endif

    root->AddArray("shaders", shaderArr);
    shaderArr->CommitType(AssetLib::Array::AS_TYPE_STRING);

    for (unsigned i = 0; i < program->GetNumInputs(); ++i)
    {
        ShaderStageRef shader = program->FindShaderStageInput(i);
        AssetLib::Asset* shaderAsset = shader->GetOwnerAsset();
        if (shaderAsset != nullptr)
        {
            AssetLib::Array::Element el;
            el.s = shaderAsset->GetPath();
            shaderArr->PushElement(el);
        }
        else
        {
            //TODO: make this a message?
            PG_FAILSTR("Cannot save a program that has a shader in memory without an asset bound");
        }
    }

    mAssetLib->BindAssetToRuntimeObject(asset, &(*program));
}
