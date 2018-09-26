#include "Dx12Pso.h"
#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

Dx12Pso::Dx12Pso(Dx12Device* device)
: Core::RefCounted(device->GetAllocator()), mDevice(device), mPso(nullptr)
{
}

Dx12Pso::~Dx12Pso()
{
    if (mPso != nullptr)
    {
        mPso->Release();
    }
}

bool Dx12Pso::Compile(const Dx12PsoDesc& desc)
{
    mDesc = desc;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoGraphicsDesc;

    if (mPso != nullptr)
    {
        mPso->Release();
        mPso = nullptr;
    }

    if (desc.program == nullptr || !desc.program->IsValid())
        return nullptr;

    psoGraphicsDesc.pRootSignature = desc.program->GetRootSignature();
    
    HRESULT result = mDevice->GetD3D()->CreateGraphicsPipelineState(&psoDesc, __uiidof(ID3D12PipelineState), reinterpret_cast<void**>(&mPso));
    if (result != S_OK)
        return false;
    else
        return true; 
}

}
}
