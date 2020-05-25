/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Pso.h
//! \author Kleber Garcia
//! \date   September 15th 2018
//! \brief  Convenience class to create PSOs

#include "Dx12Pso.h"
#include "Dx12Device.h"
#include "Dx12Resources.h"
#include <Pegasus/Utils/Memset.h>
#include <atlbase.h>

namespace Pegasus
{
namespace Render
{

Dx12GpuState::Dx12GpuState(const GpuStateConfig& config, Dx12Device* device)
:   GpuState(config, device)
{
    mGraphicsStateTemplate = {};
    Translate();
}

Dx12GpuState::~Dx12GpuState()
{
}

static D3D12_BLEND translateBlendVal(BlendVal b)
{
    switch(b)
    {
    case BlendVal::Zero:
        return D3D12_BLEND_ZERO;
    case BlendVal::One:
        return D3D12_BLEND_ONE;
    case BlendVal::SrcColor:
        return D3D12_BLEND_SRC_COLOR;
    case BlendVal::InvSrcColor:
        return D3D12_BLEND_INV_SRC_COLOR;
    case BlendVal::SrcAlpha:
        return D3D12_BLEND_SRC_ALPHA;
    case BlendVal::InvSrcAlpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case BlendVal::DestAlpha:
        return D3D12_BLEND_DEST_ALPHA;
    case BlendVal::InvDestAlpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case BlendVal::DestColor:
        return D3D12_BLEND_DEST_COLOR;
    case BlendVal::InvDestColor:
        return D3D12_BLEND_INV_DEST_COLOR;
    default:
        return D3D12_BLEND_ZERO;
    }
    return D3D12_BLEND_ZERO;
}

static D3D12_BLEND_OP translateBlendOp(BlendOp op)
{
    switch(op)
    {
        case BlendOp::Add:
            return D3D12_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return D3D12_BLEND_OP_SUBTRACT;
        case BlendOp::Min:
            return D3D12_BLEND_OP_MIN;
        case BlendOp::Max:
            return D3D12_BLEND_OP_MAX;
        default:
            return D3D12_BLEND_OP_ADD;
    }
    return D3D12_BLEND_OP_ADD;
}

static D3D12_COMPARISON_FUNC translateDethFunc(DepthComparisonFunc f)
{
    switch(f)
    {
    case DepthComparisonFunc::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
    case DepthComparisonFunc::Less:
        return D3D12_COMPARISON_FUNC_LESS;
    case DepthComparisonFunc::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case DepthComparisonFunc::LessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case DepthComparisonFunc::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
    case DepthComparisonFunc::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case DepthComparisonFunc::GreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case DepthComparisonFunc::Always:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        return D3D12_COMPARISON_FUNC_NEVER;
    }
    return D3D12_COMPARISON_FUNC_NEVER;
}

static D3D12_CULL_MODE translateCullMode(CullMode cullMode)
{
    switch (cullMode)
    {
    case CullMode::None:
        return D3D12_CULL_MODE_NONE;
    case CullMode::Cw:
        return D3D12_CULL_MODE_BACK;
    case CullMode::Ccw:
        return D3D12_CULL_MODE_FRONT;
    default:
        return D3D12_CULL_MODE_NONE;
    }
    return D3D12_CULL_MODE_NONE;
}

void Dx12GpuState::Translate()
{
    auto& s = mGraphicsStateTemplate;
    s.NumRenderTargets = mConfig.activeRenderTargets;
	s.SampleDesc.Count = 1u;
	s.SampleMask = 0xffffffff;
    //input layout
    {
        s.InputLayout.pInputElementDescs = mElementInputLayoutList; 
        s.InputLayout.NumElements = mConfig.elementCounts;
        for (unsigned elId = 0; elId < mConfig.elementCounts; ++elId)
        {
            const auto& src = mConfig.elements[elId];
            auto& elDest = mElementInputLayoutList[elId];
            elDest.SemanticName = src.semantic.c_str();
            elDest.SemanticIndex = src.semanticIndex;
            elDest.Format = GetDxFormat(src.format);
            elDest.InputSlot = src.streamId;
            elDest.AlignedByteOffset = src.byteOffset <= -1 ? D3D12_APPEND_ALIGNED_ELEMENT : (unsigned)src.byteOffset;
            elDest.InputSlotClass = src.perInstance ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            elDest.InstanceDataStepRate = src.instanceStepRate;
        }
    }

    //render targets
    {
        D3D12_BLEND_DESC& b = s.BlendState;
        for (unsigned i = 0; i < mConfig.activeRenderTargets; ++i)
        {
            const auto& src = mConfig.renderTargetInfo[i];
            auto& blendDest = b.RenderTarget[i];
            blendDest.BlendEnable = src.enableBlending;
            blendDest.SrcBlend = translateBlendVal(src.blendSrc);
            blendDest.DestBlend = translateBlendVal(src.blendDest);
            blendDest.BlendOp = translateBlendOp(src.blendOp);
            blendDest.SrcBlendAlpha = translateBlendVal(src.blendSrcAlpha);
            blendDest.DestBlendAlpha = translateBlendVal(src.blendDestAlpha);
            blendDest.BlendOpAlpha = translateBlendOp(src.blendOpAlpha);
            blendDest.RenderTargetWriteMask = (UINT8)src.colorBits;
            s.RTVFormats[i] = GetDxFormat(src.format);
        }
    }

    //ds
    {
        const auto& src = mConfig.dsDesc;
        auto& depthDest = s.DepthStencilState;
        depthDest.DepthEnable = src.depthEnable;
        depthDest.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthDest.DepthFunc = translateDethFunc(src.depthFunc);
        s.DSVFormat = GetDxFormat(src.format);
    }

    //rasterizer
    {
        auto& rasterDest = s.RasterizerState;
        rasterDest.FillMode = mConfig.enableWireFrame ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
        rasterDest.CullMode = translateCullMode(mConfig.cullMode);
        rasterDest.FrontCounterClockwise = true; //always treat front as ccw
    }

    //topolocy triangle for now.
    s.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

Dx12Pso::Dx12Pso(Dx12Device* device)
: GpuPipeline(device),
  mDevice(device),
  mProgram(device),
  mPso(nullptr),
  mType(PsoGraphics),
  mValid(false)
{
#if PEGASUS_USE_EVENTS
    SetEventUserData(nullptr);
    SetEventListener(nullptr);
#endif 
}

Dx12Pso::~Dx12Pso()
{
    if (mPso != nullptr)
    {
        mPso->Release();
    }
}

ID3D12RootSignature* Dx12Pso::GetD3DRootSignature() const
{
    return mProgram.GetRootSignature();
}

bool Dx12Pso::Compile(const GpuPipelineConfig& config)
{
    mConfig = config;
    mValid = false;
    mProgram.ClearEventData();

#if PEGASUS_USE_EVENTS
    //fwd the event listerner user data and event listeners.
    mProgram.SetEventUserData(GetEventUserData());
    mProgram.SetEventListener(GetEventListener());
#endif

    if (!mProgram.Compile(config))
        return false;

    if (mPso != nullptr)
    {
        mPso->Release();
        mPso = nullptr;
    }

    mType = PsoGraphics;

    if (mProgram.GetShaderByteCode(Dx12_Compute) != nullptr)
        mType = PsoCompute;

    if (mType == PsoGraphics)
    {
        PG_ASSERTSTR(config.graphicsState != nullptr, "Graphics state not set for PSO");
        auto graphicsState = static_cast<const Dx12GpuState*>(&(*config.graphicsState));
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = graphicsState->GetGraphicsPsoTemplate();

        psoDesc.pRootSignature = mProgram.GetRootSignature();
        for (unsigned int i = 0; i < mProgram.GetShaderByteCodeCounts(); ++i)
        {
            Dx12PipelineType type;
            CComPtr<ID3DBlob> blob = mProgram.GetShaderByteCodeByIndex(i, type);
            D3D12_SHADER_BYTECODE* targetByteCode = nullptr;
            switch (type)
            {
            case Dx12_Vertex:
                targetByteCode = &psoDesc.VS;
                break;
            case Dx12_Pixel:
                targetByteCode = &psoDesc.PS;
                break;
            case Dx12_Domain:
                targetByteCode = &psoDesc.DS;
                break;
            case Dx12_Hull:
                targetByteCode = &psoDesc.HS;
                break;
            case Dx12_Geometry:
                targetByteCode = &psoDesc.GS;
                break;
            default:
                PG_FAILSTR("Invalid shader pipeline type");
            }
            
            targetByteCode->pShaderBytecode = blob->GetBufferPointer();
            targetByteCode->BytecodeLength = blob->GetBufferSize();
        }
        
// This is only available on Warp Devices. No idea how to use it :)? 
//#if PEGASUS_DEBUG
//        psoDesc.Flags |= D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
//#endif
        
        HRESULT result = mDevice->GetD3D()->CreateGraphicsPipelineState(&psoDesc, _uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&mPso));
        if (result != S_OK)
        {
            PEGASUS_EVENT_DISPATCH(
                this, Core::CompilerEvents::LinkingEvent,
                Core::CompilerEvents::LinkingEvent::LINKING_FAIL, "Failed creating PSO. Check API logs for reason.");
            return false;
        }
        else
        {
            PEGASUS_EVENT_DISPATCH(
                this, Core::CompilerEvents::LinkingEvent,
                Core::CompilerEvents::LinkingEvent::LINKING_SUCCESS, "");
            mValid = true;
            return true; 
        }
    }
    else if (mType == PsoCompute)
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
        for (unsigned int i = 0; i < mProgram.GetShaderByteCodeCounts(); ++i)
        {
            Dx12PipelineType type;
            CComPtr<ID3DBlob> blob = mProgram.GetShaderByteCodeByIndex(i, type);
            if (type == Dx12_Compute)
            {
                psoDesc.CS.pShaderBytecode = blob->GetBufferPointer();
                psoDesc.CS.BytecodeLength = blob->GetBufferSize();
                break;
            }
        }

        psoDesc.pRootSignature = mProgram.GetRootSignature();
        HRESULT result = mDevice->GetD3D()->CreateComputePipelineState(&psoDesc, _uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&mPso));
        if (result != S_OK)
            return false;
        else
        {
            mValid = true;
            return true; 
        }
    }
    else
    {
        return false;
    }
}

bool Dx12Pso::SpaceToTableId(UINT space, Dx12ResType resType, UINT& outTableId) const
{
    if (!IsValid())
        return false;

    return mProgram.SpaceToTableId(space, resType, outTableId);
}

}
}
