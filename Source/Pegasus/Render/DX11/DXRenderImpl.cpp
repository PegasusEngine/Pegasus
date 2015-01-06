/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderImpl.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 implementation of PARR

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"

//////////////////        GLOBALS CODE BLOCK     //////////////////////////////
//         All globals holding state data are declared on this block       ////
///////////////////////////////////////////////////////////////////////////////


__declspec( thread )
struct DXState
{
    int mDispatchedProgramVersion;
    Pegasus::Render::DXProgramGPUData * mDispatchedShader;

    //TODO: implement versioning of mesh
    int dispatchedMeshVersion;
    Pegasus::Render::DXMeshGPUData    * mDispatchedMeshGpuData;
    Pegasus::Math::ColorRGBA            mClearColorValue;
	ID3D11RenderTargetView* mDispatchedTarget;
} gDXState = { 0, nullptr, 0, nullptr, Pegasus::Math::ColorRGBA(0.0, 0.0, 0.0, 0.0) };

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetProgram FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetProgram (Pegasus::Shader::ProgramLinkageInOut program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * nodeGpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::DXProgramGPUData * shaderGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGpuData);

    if (shaderGpuData->mProgramValid && (gDXState.mDispatchedShader != shaderGpuData || gDXState.mDispatchedProgramVersion != shaderGpuData->mProgramVersion))
    {
        ID3D11DeviceContext * context;
        ID3D11Device * device;
        Pegasus::Render::GetDeviceAndContext(&device, &context);
        gDXState.mDispatchedShader = shaderGpuData;
        gDXState.mDispatchedProgramVersion = shaderGpuData->mProgramVersion;
        context->VSSetShader(shaderGpuData->mVertex, nullptr, 0);
        context->PSSetShader(shaderGpuData->mPixel, nullptr, 0);
        context->DSSetShader(shaderGpuData->mDomain, nullptr, 0);
        context->HSSetShader(shaderGpuData->mHull, nullptr, 0);
        context->GSSetShader(shaderGpuData->mGeometry, nullptr, 0);
        context->CSSetShader(shaderGpuData->mCompute, nullptr, 0);
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SetMesh FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetMesh (Pegasus::Mesh::MeshInOut mesh)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Mesh::MeshData * meshData = &(*mesh->GetUpdatedMeshData());
    Pegasus::Graph::NodeGPUData * nodeGpuData = meshData->GetNodeGPUData();
    Pegasus::Render::DXMeshGPUData * meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeGpuData);
    Pegasus::Render::DXProgramGPUData * programGpuData = gDXState.mDispatchedShader;
    PG_ASSERTSTR(programGpuData != nullptr, "Must dispatch a shader first before dispatching a geometry");
    
    Pegasus::Render::DXMeshGPUData::InputLayoutEntry * inputLayoutEntry = nullptr;

    bool regenerateInputLayout = false;

    //try to find an already created layout entry if it exists
    for (int i = 0; i < meshGpuData->mInputLayoutTableCount; ++i)
    {
        if (programGpuData->mProgramGuid == meshGpuData->mInputLayoutTable[i].mProgramGuid)
        {
            inputLayoutEntry = &meshGpuData->mInputLayoutTable[i];
            if (programGpuData->mProgramVersion != meshGpuData->mInputLayoutTable[i].mProgramVersion)
            {
                meshGpuData->mInputLayoutTable[i].mInputLayout = nullptr;
                regenerateInputLayout = true;
            }
            break;
        }
    }

    //no entry found! try to allocate a new one!
    if (inputLayoutEntry == nullptr)
    {
        regenerateInputLayout = true;
        if (meshGpuData->mInputLayoutTableCapacity - 1 ==  meshGpuData->mInputLayoutTableCount)
        {
            //no more space! grow the table
            meshGpuData->mInputLayoutTableCapacity += Pegasus::Render::DXMeshGPUData::INPUT_LAYOUT_TABLE_INCREMENT;
            Pegasus::Render::DXMeshGPUData::InputLayoutEntry * newTable = PG_NEW_ARRAY (
                Pegasus::Render::GetRenderMeshFactory()->GetAllocator(),
                -1,
                "MeshGPUData inputLayoutTable",
                Pegasus::Alloc::PG_MEM_TEMP,
                Pegasus::Render::DXMeshGPUData::InputLayoutEntry,
                meshGpuData->mInputLayoutTableCapacity
            );

            //copy all the pointers
            for (int i = 0; i < meshGpuData->mInputLayoutTableCount; ++i)
            {
                Pegasus::Render::DXMeshGPUData::InputLayoutEntry& entry = meshGpuData->mInputLayoutTable[i];
                newTable[i].mInputLayout = entry.mInputLayout;
                newTable[i].mProgramGuid  = entry.mProgramGuid;
            }

            //now that pointers are safe, now delete the previous table
            PG_DELETE_ARRAY(Pegasus::Render::GetRenderMeshFactory()->GetAllocator(), meshGpuData->mInputLayoutTable);

            meshGpuData->mInputLayoutTable = newTable;
        }

        //get the next available element
        PG_ASSERT(meshGpuData->mInputLayoutTableCount < meshGpuData->mInputLayoutTableCapacity);
        inputLayoutEntry = &meshGpuData->mInputLayoutTable[meshGpuData->mInputLayoutTableCount++];
    }

    if (regenerateInputLayout)
    {
        PG_ASSERT(
                inputLayoutEntry != nullptr &&
                inputLayoutEntry->mInputLayout == nullptr &&
                programGpuData->mInputLayoutBlob->GetBufferPointer() != nullptr
        );
        HRESULT res = 
            device->CreateInputLayout(
                meshGpuData->mInputElementsDesc,
				meshGpuData->mInputElementsCount,
                programGpuData->mInputLayoutBlob->GetBufferPointer(),
                programGpuData->mInputLayoutBlob->GetBufferSize(),
                &inputLayoutEntry->mInputLayout
            );
        if (res != S_OK)
        {
            PG_LOG('WARN', "Incompatible input layout in shader and mesh");
        }
        inputLayoutEntry->mProgramGuid = programGpuData->mProgramGuid;
        inputLayoutEntry->mProgramVersion = programGpuData->mProgramVersion;
    }

	PG_ASSERT(inputLayoutEntry != nullptr);
    if (inputLayoutEntry->mInputLayout != nullptr)
    {
        context->IASetInputLayout(inputLayoutEntry->mInputLayout);
    }
    else
    {
        PG_LOG('WARN', "Incompatible input layout in shader and mesh");
    }
    
    if (gDXState.mDispatchedMeshGpuData != meshGpuData)
    {
        unsigned int strides  [MESH_MAX_STREAMS];
        unsigned int offsets[MESH_MAX_STREAMS];
        for (int i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            strides[i] = meshData->GetStreamStride(i);
            offsets[i] = 0;
        }
        context->IASetVertexBuffers (
            0,
            MESH_MAX_STREAMS,
            reinterpret_cast<ID3D11Buffer**>(meshGpuData->mVertexBuffer),
            strides,
            offsets
        );

        if (meshData->GetConfiguration().GetIsIndexed())
        {
            PG_ASSERT(meshGpuData->mIndexBuffer != nullptr);
            context->IASetIndexBuffer(
                meshGpuData->mIndexBuffer,
                DXGI_FORMAT_R16_UINT,
                0 //offset
            );
        }
        gDXState.mDispatchedMeshGpuData = meshGpuData;
    }
}


// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetRenderTargets FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRenderTargets(Pegasus::Render::RenderTarget& renderTarget, const Viewport& viewport, int renderTargetSlot)
{
    PG_ASSERTSTR(renderTargetSlot == 0, "Only supporting one render target.");
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    ID3D11DepthStencilView* depthStencil = ctx->GetDepthStencil();    
    Pegasus::Render::DXRenderTargetGPUData* rtGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget.mInternalData);
    ID3D11RenderTargetView* rt = rtGpuData->mRenderTarget;
	gDXState.mDispatchedTarget = rt;
    deviceContext->OMSetRenderTargets(
        1,
        &rt,
    nullptr//    depthStencil
    );
    
    //! TODO: optimize this by recycling the RSSetViewport dx11 call
    D3D11_VIEWPORT vp = {
        static_cast<float>(viewport.mXOffset),
        static_cast<float>(viewport.mYOffset),
        static_cast<float>(viewport.mWidth),
        static_cast<float>(viewport.mHeight),
        0.0f,
        1.0f
    };
    deviceContext->RSSetViewports(
        1,
        &vp
    );
}


// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchNullRenderTarget()
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchDefaultRenderTarget(const Pegasus::Render::Viewport& viewport)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    ID3D11RenderTargetView* rt = ctx->GetRenderTarget();
    ID3D11DepthStencilView* depthStencil = ctx->GetDepthStencil();    
    deviceContext->OMSetRenderTargets(
        1,
        &rt,
        depthStencil
    );
	gDXState.mDispatchedTarget = rt;
    
    //! TODO: optimize this by recycling the RSSetViewport dx11 call
    D3D11_VIEWPORT vp = {
        static_cast<float>(viewport.mXOffset),
        static_cast<float>(viewport.mYOffset),
        static_cast<float>(viewport.mWidth),
        static_cast<float>(viewport.mHeight),
        0.0f,
        1.0f
    };
    deviceContext->RSSetViewports(
        1,
        &vp
    );

}

///////////////////////////////////////////////////////////////////////////////
/////////////   Clear Functions                         ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Clear(bool color, bool depth, bool stencil)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    ID3D11RenderTargetView* rt = ctx->GetRenderTarget();
	ID3D11DepthStencilView* dsc = ctx->GetDepthStencil();
    if (color)
    {
        Pegasus::Math::ColorRGBA& col = gDXState.mClearColorValue;
        deviceContext->ClearRenderTargetView(
			gDXState.mDispatchedTarget,
            col.rgba
        );
    }

	if (depth)
	{
        deviceContext->ClearDepthStencilView(
            dsc,
            D3D11_CLEAR_DEPTH,
            0,
            0
        );
	}
}

void Pegasus::Render::SetClearColorValue(const Pegasus::Math::ColorRGBA& color)
{
    gDXState.mClearColorValue = color;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETRASTERIZERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRasterizerState(const RasterizerState& state)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    PG_ASSERT(state.mInternalDataAux != nullptr && state.mInternalData != nullptr);
    ID3D11RasterizerState* r = static_cast<ID3D11RasterizerState*>(state.mInternalData);
    ID3D11DepthStencilState * d = static_cast<ID3D11DepthStencilState*>(state.mInternalDataAux);
    context->RSSetState(r);
    context->OMSetDepthStencilState(d, 0);

    
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBLENDINGSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetBlendingState(const Pegasus::Render::BlendingState& blendingState)
{
    
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11BlendState* d3dState = static_cast<ID3D11BlendState*>(blendingState.mInternalData);
    context->OMSetBlendState(d3dState, NULL, 0xffffffff);
}

// ---------------------------------------------------------------------------

void Pegasus::Render::SetDepthClearValue(float d)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::SetRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
    Pegasus::Render::DXRenderTargetGPUData* rtGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget.mInternalData);
    Pegasus::Render::Viewport v;
    v.mXOffset = 0;
    v.mYOffset = 0;
    v.mWidth =  static_cast<int>(rtGpuData->mTextureView.mDesc.Width);
    v.mHeight = static_cast<int>(rtGpuData->mTextureView.mDesc.Height);
    Pegasus::Render::SetRenderTargets(renderTarget, v, 0);
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DRAW FUNCTION IMPLEMENTATION      /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Draw()
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    PG_ASSERT(gDXState.mDispatchedMeshGpuData != nullptr);
    Pegasus::Render::DXMeshGPUData* mesh = gDXState.mDispatchedMeshGpuData;
    if (mesh->mIsIndexed)
    {
        context->DrawIndexed(
            mesh->mIndexCount,
            0,
            0
        );
    }
    else
    {
        context->Draw(
            mesh->mVertexCount,
            0
        );
    }
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   GET UNIFORM FUNCTION IMPLEMENTATIONS    ///////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool UpdateUniformLocation(Pegasus::Render::DXProgramGPUData* programGPUData, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    if (programGPUData->mProgramValid && programGPUData->mReflectionData != nullptr)
    {
        for (int r = 0; r < programGPUData->mReflectionDataCount; ++r)
        {
            Pegasus::Render::DXProgramGPUData::UniformReflectionData& foundData = programGPUData->mReflectionData[r];
            if (!Pegasus::Utils::Strcmp(name, foundData.mUniformName))
            {
                outputUniform.mInternalIndex = r;
                outputUniform.mInternalOwner = programGPUData->mProgramGuid;
                outputUniform.mInternalVersion = programGPUData->mProgramVersion;
                return true;
            }
        }
    }
    
    return false;
}

static bool ProcessUpdateUniform(Pegasus::Render::Uniform& u, Pegasus::Render::DXProgramGPUData * programGpuData)
{
    if (u.mInternalVersion != programGpuData->mProgramVersion)
    {
        return UpdateUniformLocation(programGpuData, u.mName, u);
    }
    return true;
}


bool Pegasus::Render::GetUniformLocation(Pegasus::Shader::ProgramLinkageInOut program, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    PG_ASSERT(program != nullptr);
    //first attempt a constant buffer
    bool dummy = false;
    Pegasus::Graph::NodeDataRef nodeData = program->GetUpdatedData(dummy);
    Pegasus::Graph::NodeGPUData * nodeGPUData = nodeData->GetNodeGPUData();
    PG_ASSERT(nodeGPUData != nullptr);
    Pegasus::Render::DXProgramGPUData * programGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGPUData);
	outputUniform.mName[0] = '\0';
    Pegasus::Utils::Strcat(outputUniform.mName, name);
    return UpdateUniformLocation(programGPUData, name, outputUniform);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEUNIFORMBUFFER IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::CreateUniformBuffer(int size, Pegasus::Render::Buffer& outputBuffer)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11Buffer* bufferResource = nullptr;
    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = size;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    VALID_DECLARE(device->CreateBuffer(&desc, NULL, &bufferResource));
    outputBuffer.mInternalData = bufferResource;
    outputBuffer.mSize = size;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateRasterizerState(Pegasus::Render::RasterizerConfig& config, Pegasus::Render::RasterizerState& outputRasterizerState)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    outputRasterizerState.mConfig = config;

    static const D3D11_CULL_MODE cullTranslation[] = {
        D3D11_CULL_NONE,
        D3D11_CULL_FRONT,
        D3D11_CULL_BACK
    };
    D3D11_RASTERIZER_DESC rasterDesc = {
        D3D11_FILL_SOLID,
        cullTranslation[config.mCullMode],
        true, //front counter clock wise
        1, //depth bias
        1.0f, //depth bias clamp
        0.001f, //depth bias scale
        true, //always depth clip
        false, //no scissors
        false, //no multisample for now
        false  //no antialiased lines
    };
    
    ID3D11RasterizerState* rasterState = nullptr;
    VALID_DECLARE(device->CreateRasterizerState(&rasterDesc, &rasterState));
    outputRasterizerState.mInternalData = static_cast<void*>(rasterState);

    static const D3D11_COMPARISON_FUNC depthFunTranslation[] = {
        D3D11_COMPARISON_ALWAYS,
        D3D11_COMPARISON_GREATER, 
        D3D11_COMPARISON_LESS, 
        D3D11_COMPARISON_GREATER_EQUAL, 
        D3D11_COMPARISON_LESS_EQUAL, 
        D3D11_COMPARISON_EQUAL
    };
    
    D3D11_DEPTH_STENCILOP_DESC depthStencilOp = {
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_ALWAYS
    };
    
    
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {
        config.mDepthFunc != Pegasus::Render::RasterizerConfig::NONE_DF,
        D3D11_DEPTH_WRITE_MASK_ALL,
        depthFunTranslation[config.mDepthFunc],
        false, //stencil disabled
        0xff, //full stencil read mask
        0xff, //full stencil write mask
        depthStencilOp, //stencil ops
        depthStencilOp //stencil ops
    };

    ID3D11DepthStencilState* depthStencilState = nullptr;
    VALID(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));
    outputRasterizerState.mInternalDataAux = static_cast<void*>(depthStencilState);
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateBlendingState(Pegasus::Render::BlendingConfig& config, Pegasus::Render::BlendingState& blendingState)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11BlendState * d3dBlendState = nullptr;
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];

    static const D3D11_BLEND sBlendTranslator[] = {
        D3D11_BLEND_ZERO,
        D3D11_BLEND_ONE
    };

    static const D3D11_BLEND_OP sBlendOpTranslator[] = {
        D3D11_BLEND_OP_ADD, //invalid op
        D3D11_BLEND_OP_ADD, 
        D3D11_BLEND_OP_SUBTRACT, 
    };

    rtBlendDesc.BlendEnable = config.mBlendingOperator != Pegasus::Render::BlendingConfig::NONE_BO; 
    rtBlendDesc.SrcBlend = sBlendTranslator[config.mSource];
    rtBlendDesc.DestBlend = sBlendTranslator[config.mDest];
    rtBlendDesc.BlendOp = sBlendOpTranslator[config.mBlendingOperator];
    rtBlendDesc.SrcBlendAlpha = sBlendTranslator[config.mSource];
    rtBlendDesc.DestBlendAlpha = sBlendTranslator[config.mDest];
    rtBlendDesc.BlendOpAlpha = sBlendOpTranslator[config.mBlendingOperator];
    rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    VALID_DECLARE(device->CreateBlendState(&blendDesc, &d3dBlendState));
    blendingState.mInternalData = static_cast<void*>(d3dBlendState);
}
///////////////////////////////////////////////////////////////////////////////
/////////////   DELETERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRasterizerState(Pegasus::Render::RasterizerState& state)
{
    PG_ASSERT(state.mInternalDataAux != nullptr && state.mInternalData != nullptr);
    ID3D11RasterizerState* r = static_cast<ID3D11RasterizerState*>(state.mInternalData);
    ID3D11DepthStencilState * d = static_cast<ID3D11DepthStencilState*>(state.mInternalDataAux);

    r->Release();
    d->Release();
    state.mInternalDataAux = nullptr;
    state.mInternalData = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteBlendingState(Pegasus::Render::BlendingState& blendingState)
{
    PG_ASSERT(blendingState.mInternalData != nullptr);
    ID3D11BlendState* d3dBlendState = static_cast<ID3D11BlendState*>(blendingState.mInternalData);
    d3dBlendState->Release();
    blendingState.mInternalData = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBUFFER IMPLEMENTATION                ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetBuffer(Pegasus::Render::Buffer& dstBuffer, void * src, int size, int offset)
{
    ID3D11Buffer* d3dBuffer = static_cast<ID3D11Buffer*>(dstBuffer.mInternalData);
    size = size == -1 ? dstBuffer.mSize : size;
    int actualSize = size + offset;
    PG_ASSERT(d3dBuffer != nullptr);
    PG_ASSERT(actualSize == dstBuffer.mSize);
    
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    


    D3D11_MAPPED_SUBRESOURCE srd;
    if (context->Map(d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &srd) == S_OK)
    {
        Pegasus::Utils::Memcpy(srd.pData, static_cast<char*>(src) + offset, size);
        context->Unmap(d3dBuffer, 0);
    }
    else
    {
        PG_FAILSTR("Map of subresource failed");
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::DeleteBuffer(Pegasus::Render::Buffer& buffer)
{
    ID3D11Buffer * d3dBuffer = static_cast<ID3D11Buffer*>(buffer.mInternalData);
    d3dBuffer->Release();
    buffer.mSize = 0;
    buffer.mInternalData = nullptr;
}

// ---------------------------------------------------------------------------

static bool InternalSetTextureUniform(Pegasus::Render::Uniform& u, Pegasus::Render::DXTextureGPUData* texGpuData)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    PG_ASSERT(texGpuData != nullptr);
    
    Pegasus::Render::DXProgramGPUData * programData = gDXState.mDispatchedShader;
	if (programData != nullptr && programData->mProgramValid && ProcessUpdateUniform(u,programData))
    {
        ID3D11ShaderResourceView* srv = texGpuData->mSrv;
        PG_ASSERT(srv != nullptr);
        Pegasus::Render::DXProgramGPUData::UniformReflectionData& reflectionData = programData->mReflectionData[u.mInternalIndex];
		for (int s = 0; s < reflectionData.mStageCount; ++s)
        {
            Pegasus::Render::DXProgramGPUData::UniformReflectionData::StageBinding& binding = reflectionData.mStageBindings[s];
            switch(binding.mPipelineType)
            {
            case Pegasus::Shader::FRAGMENT:
                context->PSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            case Pegasus::Shader::VERTEX:
                context->VSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            case Pegasus::Shader::TESSELATION_CONTROL:
                context->HSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            case Pegasus::Shader::TESSELATION_EVALUATION:
                context->DSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            case Pegasus::Shader::GEOMETRY:
                context->GSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            case Pegasus::Shader::COMPUTE:
                context->CSSetShaderResources(binding.mBindPoint, 1, &srv);
                break;
            default:
                PG_FAIL();
            };
        }
        return true;
    }
    return false;
}

bool Pegasus::Render::SetUniformTexture(Pegasus::Render::Uniform& u, Pegasus::Texture::TextureInOut texture)
{
	Pegasus::Graph::NodeGPUData* nodeGpuData = texture->GetUpdatedTextureData()->GetNodeGPUData();
    Pegasus::Render::DXTextureGPUData * texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData, nodeGpuData);
    return InternalSetTextureUniform(u, texGpuData);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformBuffer(Pegasus::Render::Uniform& u, const Buffer& buffer)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    DXProgramGPUData * programData = gDXState.mDispatchedShader;
	if (programData != nullptr && programData->mProgramValid && ProcessUpdateUniform(u,programData))
    {
        ID3D11Buffer * d3dBuffer = static_cast<ID3D11Buffer*>(buffer.mInternalData);
        PG_ASSERT(d3dBuffer != nullptr);
		PG_ASSERT(u.mInternalIndex >= 0 && u.mInternalIndex < programData->mReflectionDataCount);
        Pegasus::Render::DXProgramGPUData::UniformReflectionData& reflectionData = programData->mReflectionData[u.mInternalIndex];
		for (int s = 0; s < reflectionData.mStageCount; ++s)
        {
            Pegasus::Render::DXProgramGPUData::UniformReflectionData::StageBinding& binding = reflectionData.mStageBindings[s];
            if (binding.mSize != buffer.mSize)
            {
                PG_LOG('WARN', "Size of cbuffer does not match. Target size of \"%s\" must be %d bytes, instead of %d bytes", u.mName, binding.mSize, buffer.mSize);
                continue;
            }
            switch(binding.mPipelineType)
            {
            case Pegasus::Shader::FRAGMENT:
                context->PSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            case Pegasus::Shader::VERTEX:
                context->VSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            case Pegasus::Shader::TESSELATION_CONTROL:
                context->HSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            case Pegasus::Shader::TESSELATION_EVALUATION:
                context->DSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            case Pegasus::Shader::GEOMETRY:
                context->GSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            case Pegasus::Shader::COMPUTE:
                context->CSSetConstantBuffers(binding.mBindPoint, 1, &d3dBuffer);
                break;
            default:
                PG_FAIL();
            };
        }
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformTextureRenderTarget(Pegasus::Render::Uniform& u, const RenderTarget& renderTarget)
{
    Pegasus::Render::DXRenderTargetGPUData * renderTargetGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget.mInternalData);
    return InternalSetTextureUniform(u, &renderTargetGpuData->mTextureView);
}

// ---------------------------------------------------------------------------
void Pegasus::Render::CleanInternalState()
{
    gDXState.mDispatchedTarget = nullptr;
    gDXState.mDispatchedMeshGpuData = nullptr;
    gDXState.mDispatchedShader = nullptr;

}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
