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



namespace PegasusShaderPrivate {


//lookup struct array with extension / GL type / pegasus Shader stage mappings
static struct PegasusExtensionMappings
{
    Pegasus::Shader::ShaderType mType;
    const char * mExtension;
} gPegasusShaderStageExtensionMappings[Pegasus::Shader::SHADER_STAGES_COUNT] =
{
    { Pegasus::Shader::VERTEX,                 ".vs"  },
    { Pegasus::Shader::FRAGMENT,               ".ps"  },
    { Pegasus::Shader::TESSELATION_CONTROL,    ".tcs" },
    { Pegasus::Shader::TESSELATION_EVALUATION, ".tes" },
    { Pegasus::Shader::GEOMETRY,               ".gs"  },
    { Pegasus::Shader::COMPUTE,                ".cs"  }
}
;

} // namespace PegasusShaderPrivate

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
}

Pegasus::Shader::ProgramLinkageReturn Pegasus::Shader::ShaderManager::CreateProgram(const char * name)
{
    Pegasus::Shader::ProgramLinkageRef program = mNodeManager->CreateNode("ProgramLinkage");
#if PEGASUS_USE_GRAPH_EVENTS 
    program->SetEventListener(mEventListener);
#endif
    program->SetFactory(mFactory);
#if PEGASUS_ENABLE_PROXIES
    //if proxies make sure to set metadata correctly
    program->SetName(name);
    mShaderTracker.InsertProgram(&(*program));
#endif
    return program; 
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::LoadShaderStageFromFile(const Pegasus::Shader::ShaderStageFileProperties& properties)
{    
    const char * extension = Pegasus::Utils::Strrchr(properties.mPath, '.');
    Pegasus::Shader::ShaderType targetStage = Pegasus::Shader::SHADER_STAGE_INVALID;
    Pegasus::Shader::ShaderStageRef stage = mNodeManager->CreateNode("ShaderStage");
    PG_ASSERT(properties.mLoader != nullptr);
    stage->SetIoManager(properties.mLoader);

#if PEGASUS_USE_GRAPH_EVENTS
    stage->SetEventListener(mEventListener);
#endif
    stage->SetFactory(mFactory);

#if PEGASUS_ENABLE_PROXIES
    stage->SetFullFilePath(properties.mPath);
#endif
    if (extension != nullptr)
    {
        for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
        {
            if (!Pegasus::Utils::Stricmp(extension, PegasusShaderPrivate::gPegasusShaderStageExtensionMappings[i].mExtension))
            {
                targetStage = PegasusShaderPrivate::gPegasusShaderStageExtensionMappings[i].mType;
                break;
            }
        }
    }
    
    if (targetStage >= 0 && targetStage < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT))
    {
        if (stage->SetSourceFromFile(targetStage, properties.mPath))
        {
#if PEGASUS_ENABLE_PROXIES
            stage->SetShaderTracker(&mShaderTracker);
            mShaderTracker.InsertShader(&(*stage));
#endif
        }
        
    }
    return stage;
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ShaderManager::CreateShaderStage(const Pegasus::Shader::ShaderStageProperties& properties)
{
    Pegasus::Shader::ShaderStageRef stage = mNodeManager->CreateNode("ShaderStage");
#if PEGASUS_USE_GRAPH_EVENTS
    stage->SetEventListener(mEventListener);
#endif
    stage->SetFactory(mFactory);

    if (properties.mType >= 0 && properties.mType < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT))
    {
        if (properties.mSource && properties.mSourceSize > 1)
        {
            stage->SetSource(properties.mType, properties.mSource, properties.mSourceSize);
#if PEGASUS_ENABLE_PROXIES
            stage->SetShaderTracker(&mShaderTracker);
            stage->SetFullFilePath("<custom-shader>");
            mShaderTracker.InsertShader(&(*stage));
#endif
        }
        else
        {
            PG_FAILSTR("Incorrect source string for shader!");
        }
    }
    else
    {
        PG_FAILSTR("Incorrect config type set");
    }
    return stage;
}
