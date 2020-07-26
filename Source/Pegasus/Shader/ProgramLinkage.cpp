#include <Pegasus/Shader/ProgramLinkage.h>
#include <Pegasus/Shader/ShaderManager.h>
#include <Pegasus/Core/Shared/CompilerEvents.h>
#include <Pegasus/Utils/String.h>
#include <Pegasus/Utils/Memcpy.h>
#include <Pegasus/AssetLib/AssetLib.h>
#include <Pegasus/AssetLib/Asset.h>
#include <Pegasus/AssetLib/ASTree.h>
#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Render/Render.h>

namespace Pegasus
{

namespace Shader
{

class ProgramLinkagePsoData : public Graph::NodeData
{
public:
    ProgramLinkagePsoData(Alloc::IAllocator* allocator, Render::IDevice* device) : Graph::NodeData(allocator)
    {
        mGpuPipeline = device->CreateGpuPipeline();

        Render::GpuStateConfig gpuConfig;
        gpuConfig.elementCounts = 2;
        gpuConfig.elements[0].semantic = "POSITION";
        gpuConfig.elements[0].format = Core::FORMAT_RGBA_32_FLOAT;
	    gpuConfig.elements[1].semantic = "NORMAL";
	    gpuConfig.elements[1].format = Core::FORMAT_RGBA_32_FLOAT;
        mGpuState = device->CreateGpuState(gpuConfig);

    }

    virtual ~ProgramLinkagePsoData() {}

    Render::GpuPipelineRef& GetGpuPipeline() { return mGpuPipeline; }
    Render::GpuStateRef& GetGpuState() { return mGpuState; }

    bool Compile(const Render::GpuPipelineConfig& pipelineConfig) { return mGpuPipeline->Compile(pipelineConfig); }

private:
    Render::GpuPipelineRef mGpuPipeline;
    Render::GpuStateRef mGpuState;
    Alloc::IAllocator* mAllocator;
};


ProgramLinkage::ProgramLinkage(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
: Graph::OperatorNode(nodeAllocator, nodeDataAllocator)
, AssetLib::RuntimeAssetObject(this)
#if PEGASUS_ENABLE_PROXIES
  //initializing proxy
, mProxy(this)
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER
}

ProgramLinkage::~ProgramLinkage()
{
    if (mShaderSource != nullptr)
        mShaderSource->UnregisterParent(this);

#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_DESTROY_USER_DATA(&mProxy, "ProgramLinkage", GetEventListener());
#endif
}

Graph::NodeData* ProgramLinkage::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "ProgramData", Alloc::PG_MEM_TEMP) ProgramLinkagePsoData(GetNodeDataAllocator(), mManager->GetRenderDevice());
}

void ProgramLinkage::Compile()
{
    Node::GetUpdatedData();
}

void ProgramLinkage::GenerateData()
{
    PG_ASSERT(GetData() != nullptr);
    if (mShaderSource == nullptr)
    {
#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "Missing source for GPU Program data \"%s\"", GetName());
#else
        PG_LOG('ERR_', "Missing source for GPU Program data.");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG
        return;
    }

#if PEGASUS_ENABLE_DETAILED_LOG
#if PEGASUS_ENABLE_PROXIES
    PG_LOG('SHDR', "Generating the GPU data of program linkage \"%s\"", GetName());
#else
    PG_LOG('SHDR', "Generating the GPU data of a program linkage");
#endif
#endif  // PEGASUS_ENABLE_DETAILED_LOG

    auto* programData = static_cast<ProgramLinkagePsoData*>(GetData());
    
#if PEGASUS_USE_EVENTS
	programData->GetGpuPipeline()->SetEventUserData(mShaderSource->GetEventUserData());
    programData->GetGpuPipeline()->SetEventListener(GetEventListener());
#endif

	Render::GpuPipelineConfig gpuPipelineConfig;

    const char* srcCode = nullptr;
    int srcCodeSz = 0u;
    mShaderSource->GetSource(&srcCode, srcCodeSz);
    gpuPipelineConfig.source = srcCode;
    gpuPipelineConfig.sourceSize = (unsigned)srcCodeSz;
	gpuPipelineConfig.mainNames[Render::Pipeline_Vertex] = "vsMain";
	gpuPipelineConfig.mainNames[Render::Pipeline_Pixel] = "psMain";
    gpuPipelineConfig.graphicsState = programData->GetGpuState();
    programData->Compile(gpuPipelineConfig);

#if PEGASUS_USE_EVENTS
	programData->GetGpuPipeline()->SetEventUserData(nullptr);
	programData->GetGpuPipeline()->SetEventListener(nullptr);
#endif
}

void ProgramLinkage::SetSourceCode(ShaderSourceRef& source)
{
    InvalidateData();
    if (source == nullptr && mShaderSource != nullptr)
    {
        mShaderSource->UnregisterParent(this);
    }

    mShaderSource = source;
    mShaderSource->RegisterParent(this);
}

void ProgramLinkage::AddInput(Graph::NodeIn node)
{
    PG_FAILSTR("Add input call not allowed for shader linkage! The topology must not be messed up with.");
}

Graph::NodeReturn ProgramLinkage::CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ProgramLinkage", Alloc::PG_MEM_TEMP) ProgramLinkage(nodeManager, nodeAllocator, nodeDataAllocator);
}

#if PEGASUS_ENABLE_PROXIES
const char* ProgramLinkage::GetName() const
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

bool ProgramLinkage::OnReadAsset(AssetLib::AssetLib* lib, const AssetLib::Asset* asset)
{
    const AssetLib::Object* root = asset->Root();
    int nameId = root->FindString("name");
#if PEGASUS_ENABLE_PROXIES
    SetName(root->GetString(nameId));
#endif

    int shaderSourceId = root->FindAsset("shaderSource");
    if (shaderSourceId != -1)
    {
        AssetLib::RuntimeAssetObjectRef assetRef = root->GetAsset(shaderSourceId);
        if (assetRef != nullptr && assetRef->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == ASSET_TYPE_SHADER_SRC.mTypeGuid)
        {
            Shader::ShaderSourceRef shaderSrc =  static_cast<Pegasus::Shader::ShaderSource*>(&(*assetRef));
            SetSourceCode(shaderSrc);
        }
        else
        {
            PG_LOG('ERR_', "Invalid asset type for asset: %s", assetRef->GetOwnerAsset()->GetPath());
        }
    }

    return true;
}

void ProgramLinkage::OnWriteAsset(AssetLib::AssetLib* lib, AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();    
#if PEGASUS_ENABLE_PROXIES
    root->AddString("name", GetName());
#endif

    Shader::ShaderSourceRef shaderSrc = GetSourceCode();
    if (shaderSrc != nullptr && shaderSrc->GetOwnerAsset() != nullptr)
    {
        root->AddAsset("shaderSource", shaderSrc);
    }
}

}
}
