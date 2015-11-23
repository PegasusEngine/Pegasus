#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Shader/IShaderFactory.h"
#include "Pegasus/Shader/ShaderTracker.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"


//! private data structures
namespace PegasusShaderPrivate {

unsigned char ToStageFlag(Pegasus::Shader::ShaderType type)
{
    return (1 << static_cast<unsigned char>(type));
}

} // namespace PegasusShaderPrivate

Pegasus::Shader::ProgramLinkage::ProgramLinkage(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:
Pegasus::Graph::OperatorNode(nodeAllocator, nodeDataAllocator), mStageFlags(0), mFactory(nullptr)
    ,AssetLib::RuntimeAssetObject(this)
#if PEGASUS_ENABLE_PROXIES
    //initializing proxy
    ,mProxy(this)
    ,mShaderTracker(nullptr)
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER
}

Pegasus::Shader::ProgramLinkage::~ProgramLinkage()
{

    for (unsigned i = 0; i < GetNumInputs(); ++i)
    {
        Pegasus::Shader::ShaderStageRef stage = FindShaderStageInput(i);
        stage->UnregisterParent(this);
    }
    if (GetData() != nullptr)
    {
        mFactory->DestroyProgramGPUData(&(*GetData()));
    }

#if PEGASUS_ENABLE_PROXIES
    if (mShaderTracker != nullptr)
    {
        mShaderTracker->DeleteProgram(this);
    }
#endif

#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_DESTROY_USER_DATA(&mProxy, "ProgramLinkage", GetEventListener());
#endif
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

void Pegasus::Shader::ProgramLinkage::InvalidateData()
{
    Pegasus::Graph::OperatorNode::InvalidateData();
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

        shaderStage->RegisterParent(this);
    }
}

Pegasus::Shader::ShaderStageRef Pegasus::Shader::ProgramLinkage::RemoveShaderStage(Pegasus::Shader::ShaderType type)
{
    Pegasus::Shader::ShaderStageRef ref;

    ref = FindShaderStage(type);
    if (ref != nullptr)
    {
        ref->UnregisterParent(this);
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

Pegasus::Graph::NodeReturn Pegasus::Shader::ProgramLinkage::CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ProgramLinkage", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ProgramLinkage(nodeManager, nodeAllocator, nodeDataAllocator);
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

#if PEGASUS_ENABLE_PROXIES
const char* Pegasus::Shader::ProgramLinkage::GetName() const
{
    if (GetOwnerAsset() != nullptr)
    {
        return GetOwnerAsset()->GetName();
    }
    else
    {
        return "";
    }
}
#endif

bool Pegasus::Shader::ProgramLinkage::OnReadAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();
    int nameId = root->FindString("name");
    int shaders = root->FindArray("shaders");

    if (shaders != -1)
    {
        AssetLib::Array* shaderArr = root->GetArray(shaders);
        if (shaderArr->GetType() == AssetLib::Array::AS_TYPE_STRING)
        {
            for (int i = 0; i < shaderArr->GetSize(); ++i)
            {
                const AssetLib::Array::Element& element = shaderArr->GetElement(i);
                Pegasus::Shader::ShaderStageRef shaderStage = mManager->LoadShader(element.s);
                if (shaderStage != nullptr)
                {
                    SetShaderStage(shaderStage);
                }
            }
        }
        else if (shaderArr->GetSize() != 0) //forgivable error if empty list
        {
            PG_LOG('ERR_', "Invalid list of shaders");
            return false;
        }
    }

    return true;
}

void Pegasus::Shader::ProgramLinkage::OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();    

    AssetLib::Array* shaderArr = asset->NewArray();

    root->AddArray("shaders", shaderArr);
#if PEGASUS_ENABLE_PROXIES
    root->AddString("name", GetName());
#endif
    shaderArr->CommitType(AssetLib::Array::AS_TYPE_STRING);

    for (unsigned i = 0; i < GetNumInputs(); ++i)
    {
        ShaderStageRef shader = FindShaderStageInput(i);
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

}

