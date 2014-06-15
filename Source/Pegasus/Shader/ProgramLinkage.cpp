#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/IShaderFactory.h"

#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"

//! private data structures
namespace PegasusShaderPrivate {

unsigned char ToStageFlag(Pegasus::Shader::ShaderType type)
{
    return (1 << static_cast<unsigned char>(type));
}

} // namespace PegasusShaderPrivate

Pegasus::Shader::ProgramLinkage::ProgramLinkage(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:
Pegasus::Graph::OperatorNode(nodeAllocator, nodeDataAllocator), mStageFlags(0), mFactory(nullptr)
#if PEGASUS_ENABLE_PROXIES
    //initializing proxy
    ,mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER
}

Pegasus::Shader::ProgramLinkage::~ProgramLinkage()
{
    if (GetData() != nullptr)
    {
        mFactory->DestroyProgramGPUData(&(*GetData()));
    }
}

void Pegasus::Shader::ProgramLinkage::ReleaseDataAndPropagate()
{
    if (GetData() != nullptr)
    {
#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('SHDR', "Destroying the GPU data of program linkage \"%s\"", GetName());
#else
        PG_LOG('SHDR', "Destroying the GPU data of a program linkage");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG

        mFactory->DestroyProgramGPUData(&(*GetData()));
    }
}

Pegasus::Shader::ShaderStageRef Pegasus::Shader::ProgramLinkage::GetShaderStage(Pegasus::Shader::ShaderType type) const
{
    PG_ASSERT(type >= 0 && type < Pegasus::Shader::SHADER_STAGES_COUNT);
    return FindShaderStage(type);
}

Pegasus::Graph::NodeData* Pegasus::Shader::ProgramLinkage::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "ProgramData", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Graph::NodeData(GetNodeDataAllocator());
}

void Pegasus::Shader::ProgramLinkage::GenerateData()
{
    PG_ASSERT(GetData() != nullptr);

#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
    PG_LOG('SHDR', "Generating the GPU data of program linkage \"%s\"", GetName());
#else
    PG_LOG('SHDR', "Generating the GPU data of a program linkage");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG
        
    mFactory->GenerateProgramGPUData(&(*this), &(*GetData()));
}

void Pegasus::Shader::ProgramLinkage::SetShaderStage(Pegasus::Shader::ShaderStageIn shaderStage)
{
    if (shaderStage != nullptr)
    {
        unsigned char flag = PegasusShaderPrivate::ToStageFlag(shaderStage->GetStageType());
        if (flag & mStageFlags)
        {
            RemoveShaderStage(shaderStage->GetStageType());
        }

        PG_ASSERT(shaderStage->GetStageType() >= 0 && shaderStage->GetStageType() < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT));
        OperatorNode::AddInput(shaderStage);
        mStageFlags |= flag;
    }
}

Pegasus::Shader::ShaderStageRef Pegasus::Shader::ProgramLinkage::RemoveShaderStage(Pegasus::Shader::ShaderType type)
{
    Pegasus::Shader::ShaderStageRef ref;

    ref = FindShaderStage(type);
    if (ref != nullptr)
    {
        RemoveInput(ref);
    }

    return ref;
}

void Pegasus::Shader::ProgramLinkage::OnRemoveInput(unsigned int index)
{
    Pegasus::Shader::ShaderStageRef targetStage = GetInput(index);
    unsigned char flag =  PegasusShaderPrivate::ToStageFlag(targetStage->GetStageType());
    mStageFlags &= ~(flag); //turn off bit
}

void Pegasus::Shader::ProgramLinkage::AddInput(Pegasus::Graph::NodeIn node)
{
    PG_FAILSTR("Add input call not allowed for shader linkage! The topology must not be messed up with.");
}

#if PEGASUS_ENABLE_PROXIES
void Pegasus::Shader::ProgramLinkage::SetName(const char * name)
{
    int len = 0;
    if (name)
    {
        int namelen = Pegasus::Utils::Strlen(name); 
        len = namelen < Pegasus::Shader::ProgramLinkage::METADATA_NAME_LENGTH - 1 ? namelen : Pegasus::Shader::ProgramLinkage::METADATA_NAME_LENGTH - 1;
        Pegasus::Utils::Memcpy(mName, name, len);
    }
    mName[len] = '\0';
}
#endif

Pegasus::Graph::NodeReturn Pegasus::Shader::ProgramLinkage::CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ProgramLinkage", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ProgramLinkage(nodeAllocator, nodeDataAllocator);
}

Pegasus::Shader::ShaderStageReturn Pegasus::Shader::ProgramLinkage::FindShaderStage(Pegasus::Shader::ShaderType type) const
{
    for (unsigned int i = 0; i < GetNumInputs(); ++i)
    {
        Pegasus::Shader::ShaderStageRef stage = GetInput(i);    
        if (stage->GetStageType() == type)
        {
            return stage;
        }
    }
    return nullptr;
}


