#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Shader/ShaderManager.h"
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
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER
}

Pegasus::Shader::ProgramLinkage::~ProgramLinkage()
{

    if (GetData() != nullptr)
    {
    }

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

    }
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
        
}

void Pegasus::Shader::ProgramLinkage::AddInput(Pegasus::Graph::NodeIn node)
{
    PG_FAILSTR("Add input call not allowed for shader linkage! The topology must not be messed up with.");
}

Pegasus::Graph::NodeReturn Pegasus::Shader::ProgramLinkage::CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ProgramLinkage", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ProgramLinkage(nodeManager, nodeAllocator, nodeDataAllocator);
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

bool Pegasus::Shader::ProgramLinkage::OnReadAsset(Pegasus::AssetLib::AssetLib* lib, const Pegasus::AssetLib::Asset* asset)
{
    const AssetLib::Object* root = asset->Root();
    int nameId = root->FindString("name");
    return true;
}

void Pegasus::Shader::ProgramLinkage::OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, Pegasus::AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();    
#if PEGASUS_ENABLE_PROXIES
    root->AddString("name", GetName());
#endif
}

