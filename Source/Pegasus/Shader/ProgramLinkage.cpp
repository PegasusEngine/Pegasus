#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderData.h"

//! private data structures
namespace PegasusShaderPrivate {

unsigned char ToStageFlag(Pegasus::Shader::ShaderType type)
{
    return (1 << static_cast<unsigned char>(type));
}

} // namespace PegasusShaderPrivate

Pegasus::Shader::ProgramLinkage::ProgramLinkage(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:
Pegasus::Graph::OperatorNode(nodeAllocator, nodeDataAllocator), mStageFlags(0)
{

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
    //TODO optimize this code in size. Loops are expensive in instruction #s??
    PG_ASSERT(GetData() != nullptr);
    Pegasus::Shader::ProgramDataRef programData = GetData();
    const int shaderStagesCount = Pegasus::Shader::SHADER_STAGES_COUNT;
    ShaderHandle programDataPipe[shaderStagesCount];
    for (unsigned int i = 0; i < shaderStagesCount; ++i)
    {
        programDataPipe[i] = Pegasus::Shader::INVALID_SHADER_HANDLE;//initialize to invalid shader handles
    }
    
    bool dummy = false;
    for (unsigned int i = 0; i < GetNumInputs(); ++i)
    {
        Pegasus::Shader::ShaderStageRef shaderStage = GetInput(i);
        if (shaderStage->GetStageType() != Pegasus::Shader::SHADER_STAGE_INVALID)
        {
            Pegasus::Shader::ShaderDataRef shaderData = shaderStage->GetUpdatedData(dummy);    
            programDataPipe[shaderStage->GetStageType()] = shaderData->GetShaderHandle();
        }
    }

    bool success = mInternalLinker.Link(programDataPipe);
    
    if (success)
    {
        programData->SetHandle(mInternalLinker.GetProgramHandle());     
    }
    else
    {
        programData->SetHandle(Pegasus::Shader::INVALID_SHADER_HANDLE);
    }
}

#if PEGASUS_SHADER_USE_EDIT_EVENTS
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


void Pegasus::Shader::ProgramLinkage::SetUserData(Pegasus::Shader::IUserData * userData)
{
    mInternalLinker.SetUserData(userData);
}
#endif //PEGASUS_SHADER_USE_EDIT_EVENTS

void Pegasus::Shader::ProgramLinkage::SetShaderStage(Pegasus::Shader::ShaderStageIn shaderStage)
{
    if (shaderStage != nullptr)
    {
        unsigned char flag = PegasusShaderPrivate::ToStageFlag(shaderStage->GetStageType());
        if (flag & mStageFlags)
        {
            RemoveInput(FindShaderStage(shaderStage->GetStageType()));
        }

        PG_ASSERT(shaderStage->GetStageType() >= 0 && shaderStage->GetStageType() < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT));
        OperatorNode::AddInput(shaderStage);
        mStageFlags |= flag;
    }
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


