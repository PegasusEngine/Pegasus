#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderData.h"

//! private data structures
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

Pegasus::Shader::ProgramLinkage::ProgramLinkage(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:
Pegasus::Graph::OperatorNode(nodeAllocator, nodeDataAllocator)
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        Pegasus::Shader::ShaderStageRef invalidShaderStage = 
            PG_NEW(GetNodeDataAllocator(), -1, "ShaderData", Pegasus::Alloc::PG_MEM_TEMP) 
            Pegasus::Shader::ShaderStage(nodeAllocator, nodeDataAllocator);
        Node::AddInput(invalidShaderStage);
    }
}


Pegasus::Shader::ProgramLinkage::~ProgramLinkage()
{
}

Pegasus::Shader::ShaderStageRef Pegasus::Shader::ProgramLinkage::GetShaderStage(Pegasus::Shader::ShaderType type) const
{
    PG_ASSERT(type >= 0 && type < Pegasus::Shader::SHADER_STAGES_COUNT);
    return GetInput(static_cast<unsigned int>(type));
}

Pegasus::Graph::NodeData* Pegasus::Shader::ProgramLinkage::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "ProgramData", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ProgramData(GetNodeDataAllocator());
}

void Pegasus::Shader::ProgramLinkage::GenerateData()
{
    PG_ASSERT(GetData() != nullptr);
    Pegasus::Shader::ProgramDataRef programData = GetData();
    const int shaderStagesCount = Pegasus::Shader::SHADER_STAGES_COUNT;
    GLuint programDataPipe[shaderStagesCount];
    bool dummy = false;
    for (int i = 0; i < shaderStagesCount; ++i)
    {
        Pegasus::Shader::ShaderDataRef shaderData = this->GetInput(i)->GetUpdatedData(dummy);
        programDataPipe[i] = shaderData->GetGlHandle();
    }

    bool success = mInternalLinker.Link(programDataPipe);
    
    if (success)
    {
        programData->SetGlHandle(mInternalLinker.GetProgramHandle());     
    }
    else
    {
        programData->SetGlHandle(0);
    }
    
}

void Pegasus::Shader::ProgramLinkage::SetEventListener(Pegasus::Shader::IEventListener * eventListener)
{
    //propagate event listener all the way down to compiler
    mInternalLinker.SetEventListener(eventListener);
    for (unsigned int i = 0; i < GetMaxNumInputNodes(); ++i)
    {
        Pegasus::Shader::ShaderStageRef stage = GetInput(i);
        stage->SetEventListener(eventListener);
    }
}


void Pegasus::Shader::ProgramLinkage::SetShaderStage(const Pegasus::Shader::ShaderStageConfig& config)
{
    if (config.mType >= 0 && config.mType < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT))
    {
        Pegasus::Shader::ShaderStageRef stage = GetShaderStage(config.mType);
        if (config.mUserData != nullptr)
        {
            stage->SetUserData (config.mUserData);
        }

        if (config.mSource && config.mSourceSize > 1)
        {
            stage->SetSource(config.mType, config.mSource, config.mSourceSize);
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
}


bool Pegasus::Shader::ProgramLinkage::LoadShaderStage(const char * path, Io::IOManager * loader)
{
    const char * extension = strrchr(path, '.');
    Pegasus::Shader::ShaderType targetStage = Pegasus::Shader::SHADER_STAGE_INVALID;
    if (extension != nullptr)
    {
        for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
        {
            if (!_stricmp(extension, PegasusShaderPrivate::gPegasusShaderStageExtensionMappings[i].mExtension))
            {
                targetStage = PegasusShaderPrivate::gPegasusShaderStageExtensionMappings[i].mType;
                break;
            }
        }
    }
    
    Pegasus::Shader::ShaderStageRef stage = GetShaderStage(targetStage);
    return stage->SetSourceFromFile(targetStage, path, loader);
}


void Pegasus::Shader::ProgramLinkage::AddInput(Pegasus::Graph::NodeIn node)
{
    PG_FAILSTR("Add input call not allowed for shader linkage! The topology must not be messed up with.");
}


Pegasus::Graph::NodeReturn Pegasus::Shader::ProgramLinkage::CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ProgramLinkage", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ProgramLinkage(nodeAllocator, nodeDataAllocator);
}


