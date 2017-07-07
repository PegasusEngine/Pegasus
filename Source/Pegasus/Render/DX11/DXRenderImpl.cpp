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

#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/String.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"

/// MACROS ///
#define MAX_UAV_SLOT_COUNT 8

#define RENDER_NEW(__type) \
        PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, #__type, Pegasus::Alloc::PG_MEM_PERM) __type(Pegasus::Memory::GetRenderAllocator())

#define RENDER_NEW_GPU_DATA(__type) \
        PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, #__type, Pegasus::Alloc::PG_MEM_PERM) __type()

//////////////

//////////////////        GLOBALS CODE BLOCK     //////////////////////////////
//         All globals holding state data are declared on this block       ////
///////////////////////////////////////////////////////////////////////////////

// Include inline format description //
DXGI_FORMAT GetDxFormat(Pegasus::Core::Format format);
#include "../Source/Pegasus/Render/DX11/DXPegasusFormat.inl"

// Format reading end.  //


__declspec( thread )
struct DXState
{
    int mDispatchedProgramVersion;
    Pegasus::Render::DXProgramGPUData * mDispatchedShader;

    //TODO: implement versioning of mesh
    int mDispatchedMeshVersion;
    Pegasus::Render::DXMeshGPUData    * mDispatchedMeshGpuData;
    Pegasus::Math::ColorRGBA            mClearColorValue;
    Pegasus::Render::PrimitiveMode      mPrimitiveMode;
    int mTargetsCount;    
    float mDepthClearVal;
    ID3D11RenderTargetView* mDispatchedTargets[Pegasus::Render::Constants::MAX_RENDER_TARGETS];
    ID3D11DepthStencilView* mDispatchedDepth;
    ID3D11UnorderedAccessView* mComputeOutputs[MAX_UAV_SLOT_COUNT];
    bool mComputeOutputsDirty;
    int  mComputeOutputsCount;

} gDXState;// = { 0, nullptr, 0, nullptr, Pegasus::Math::ColorRGBA(0.0, 0.0, 0.0, 0.0), Pegasus::Render::PM_AUTOMATIC };

        
       
const D3D_PRIMITIVE_TOPOLOGY gPrimitiveModeTranslation[] = {
     D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    ,D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    ,D3D_PRIMITIVE_TOPOLOGY_LINELIST
    ,D3D_PRIMITIVE_TOPOLOGY_LINESTRIP
    ,D3D_PRIMITIVE_TOPOLOGY_POINTLIST
};
// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   SetProgram FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetProgram (Pegasus::Shader::ProgramLinkageInOut program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * nodeGpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::DXProgramGPUData * shaderGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGpuData);
    if (shaderGpuData->mProgramValid)
    {
        if (gDXState.mDispatchedShader != shaderGpuData || gDXState.mDispatchedProgramVersion != shaderGpuData->mProgramVersion)
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
            //set the global uniforms this program might have.
            for (int i = 0; i < shaderGpuData->mGlobalUniformCount; ++i)
            {
                SetUniformBuffer(shaderGpuData->mGlobalUniforms[i], shaderGpuData->mGlobalBuffers[i]);            
            }
        }
    }
    else
    {
        gDXState.mDispatchedShader = nullptr;
        gDXState.mDispatchedProgramVersion = 0;
    }
}

int Pegasus::Render::GetProgramVersion (Pegasus::Shader::ProgramLinkageInOut program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * nodeGpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::DXProgramGPUData * shaderGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGpuData);
    return shaderGpuData->mProgramVersion;
}


///////////////////////////////////////////////////////////////////////////////
/////////////   Samplers FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const D3D11_FILTER gFilterTranslation[]=
{
    D3D11_FILTER_MIN_MAG_MIP_POINT,
    D3D11_FILTER_MIN_MAG_MIP_LINEAR,
    D3D11_FILTER_ANISOTROPIC
};

static const D3D11_TEXTURE_ADDRESS_MODE gCoordTranslation[]=
{
    D3D11_TEXTURE_ADDRESS_WRAP,
    D3D11_TEXTURE_ADDRESS_MIRROR,
    D3D11_TEXTURE_ADDRESS_CLAMP,
    D3D11_TEXTURE_ADDRESS_BORDER
};

Pegasus::Render::SamplerStateRef Pegasus::Render::CreateSamplerState(const Pegasus::Render::SamplerStateConfig& config)
{
    Pegasus::Render::SamplerStateRef sampler = RENDER_NEW(Pegasus::Render::SamplerState);
    Pegasus::Render::DXSampler* d3ds = RENDER_NEW_GPU_DATA(Pegasus::Render::DXSampler);
    D3D11_SAMPLER_DESC& desc = d3ds->mDesc;
    desc.Filter = gFilterTranslation[config.mFilter];
    desc.AddressU = gCoordTranslation[config.mUCoordMode];
    desc.AddressV = gCoordTranslation[config.mVCoordMode];
    desc.AddressW = gCoordTranslation[config.mWCoordMode];
    desc.MipLODBias = (float)config.mMipBias;
    desc.MaxAnisotropy = config.mMaxAnisotropy == -1 ? 16 : (unsigned)config.mMaxAnisotropy;
    desc.MinLOD = config.mMinMip == -1 ? 0.0f : (float)config.mMinMip;
    desc.MaxLOD = config.mMaxMip == -1 ? D3D11_FLOAT32_MAX : (float)config.mMaxMip;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    Utils::Memcpy(&desc.BorderColor, &config.mBorderColor, sizeof(desc.BorderColor));

    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    VALID_DECLARE(device->CreateSamplerState(&d3ds->mDesc, &d3ds->mSampler));
    sampler->SetInternalData(d3ds);
    return sampler;
}

void Pegasus::Render::SetComputeSampler(Pegasus::Render::SamplerStateRef& sampler, int slot)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Render::DXSampler* d3ds = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXSampler, sampler->GetInternalData());
    ID3D11SamplerState * ss = d3ds->mSampler;
    context->CSSetSamplers(0,1,&ss);
}
bool gDebug = false;
void Pegasus::Render::SetPixelSampler(Pegasus::Render::SamplerStateRef& sampler, int slot)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Render::DXSampler* d3ds = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXSampler, sampler->GetInternalData());
    ID3D11SamplerState * ss = d3ds->mSampler;
    context->PSSetSamplers(0,1,&ss);
}

void Pegasus::Render::SetVertexSampler(Pegasus::Render::SamplerStateRef& sampler, int slot)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Render::DXSampler* d3ds = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXSampler, sampler->GetInternalData());
    ID3D11SamplerState * ss = d3ds->mSampler;
    context->VSSetSamplers(0,1,&ss);
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::SamplerStateConfig>::~BasicResource()
{
    Pegasus::Render::DXSampler* d3ds = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXSampler, GetInternalData());
    PG_DELETE(Pegasus::Memory::GetRenderAllocator(), d3ds);
}


///////////////////////////////////////////////////////////////////////////////
/////////////   SetMesh FUNCTION IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetMesh (Pegasus::Mesh::MeshInOut mesh)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Mesh::MeshDataRef meshData = mesh->GetUpdatedMeshData();
    if (meshData == nullptr)
    {
        PG_LOG('ERR_',"Attempting to set invalid node data.");
        return;
    }

    Pegasus::Graph::NodeGPUData * nodeGpuData = meshData->GetNodeGPUData();
    Pegasus::Render::DXMeshGPUData * meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeGpuData);
    Pegasus::Render::DXProgramGPUData * programGpuData = gDXState.mDispatchedShader;

    if (programGpuData == nullptr)
    {
        PG_LOG('ERR_', "Must dispatch a program before trying to set a mesh.");
        return;
    } 
    
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
        if(programGpuData->mInputLayoutBlob == nullptr )
        {
            PG_LOG('ERR_',"Cannot set mesh while not binding a valid shader.");
            return;
        }
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
            PG_LOG('ERR_', "Incompatible input layout in shader and mesh");
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
        PG_LOG('ERR_', "Incompatible input layout in shader and mesh");
    }
    
    {
        unsigned int strides  [MESH_MAX_STREAMS];
        unsigned int offsets[MESH_MAX_STREAMS];
        ID3D11Buffer* buffers[MESH_MAX_STREAMS];
        for (int i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            strides[i] = meshData->GetStreamStride(i);            
            buffers[i] = meshGpuData->mVertexStreams[i].mBuffer;
            offsets[i] = 0;
        }

        context->IASetVertexBuffers (
            0,
            MESH_MAX_STREAMS,
            buffers,
            strides,
            offsets
        );

        if (meshData->GetConfiguration().GetIsIndexed())
        {
            PG_ASSERT(meshGpuData->mIndexStream.mBuffer != nullptr);
            context->IASetIndexBuffer(
                meshGpuData->mIndexStream.mBuffer,
                DXGI_FORMAT_R16_UINT,
                0 //offset
            );
        }
        gDXState.mDispatchedMeshGpuData = meshGpuData;
    }
}

void Pegasus::Render::UnbindMesh()
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    gDXState.mDispatchedMeshGpuData = nullptr;
    unsigned int strides  [MESH_MAX_STREAMS];
    unsigned int offsets[MESH_MAX_STREAMS];
    ID3D11Buffer* buffers[MESH_MAX_STREAMS];
    Utils::Memset32(buffers, 0, sizeof(buffers));
    Utils::Memset32(offsets, 0, sizeof(offsets));
    Utils::Memset32(offsets, 0, sizeof(strides));
    context->IASetVertexBuffers(0,MESH_MAX_STREAMS,buffers, strides, offsets);
    context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
}

Pegasus::Render::BufferRef Pegasus::Render::GetIndexBuffer(Pegasus::Mesh::MeshDataRef nodeData)
{
    PG_ASSERT(nodeData->GetConfiguration().GetIsIndexed());
    Pegasus::Render::DXMeshGPUData * meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeData->GetNodeGPUData());
    return meshGpuData->mIndexBuffer;
}

Pegasus::Render::BufferRef Pegasus::Render::GetVertexBuffer(Pegasus::Mesh::MeshDataRef nodeData, int streamId)
{
    PG_ASSERT(streamId >= 0 && streamId < MESH_MAX_STREAMS);
    Pegasus::Render::DXMeshGPUData * meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeData->GetNodeGPUData());
    return meshGpuData->mVertexBuffers[streamId];
}

Pegasus::Render::BufferRef Pegasus::Render::GetDrawIndirectBuffer(Pegasus::Mesh::MeshDataRef nodeData)
{
    Pegasus::Render::DXMeshGPUData * meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeData->GetNodeGPUData());
    return meshGpuData->mDrawIndirectBuffer;
}

// ---------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/////////////   SetViewport FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetViewport(const Pegasus::Render::Viewport& viewport)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
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

void Pegasus::Render::SetViewport(const Pegasus::Render::RenderTargetRef& viewport)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    D3D11_VIEWPORT vp = {
        0.0f,
        0.0f,
        static_cast<float>(viewport->GetConfig().mWidth),
        static_cast<float>(viewport->GetConfig().mHeight),
        0.0f,
        1.0f
    };
    deviceContext->RSSetViewports(
        1,
        &vp
    );
}

void Pegasus::Render::SetViewport(const Pegasus::Render::DepthStencilRef& viewport)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
}

// ---------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/////////////   SetRenderTargets FUNCTION IMPLEMENTATION //////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRenderTarget (Pegasus::Render::RenderTargetRef& renderTarget)
{
    Pegasus::Render::SetRenderTargets(1, &renderTarget);
}

void Pegasus::Render::SetRenderTarget (Pegasus::Render::RenderTargetRef& renderTarget, Pegasus::Render::DepthStencilRef& depthStencil)
{
    Pegasus::Render::SetRenderTargets(1, &renderTarget, depthStencil);
}

void Pegasus::Render::SetRenderTargets (int renderTargetCount, Pegasus::Render::RenderTargetRef* renderTarget)
{
    DepthStencilRef ds = nullptr;
    Pegasus::Render::SetRenderTargets(renderTargetCount, renderTarget, ds);
}

void Pegasus::Render::SetRenderTargets (int renderTargetNum, Pegasus::Render::RenderTargetRef* renderTarget, Pegasus::Render::DepthStencilRef& depthStencil)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    PG_ASSERTSTR(ctx != nullptr, "must bind a context!!");
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();

    PG_ASSERT(renderTargetNum >= 0 && renderTargetNum < Pegasus::Render::Constants::MAX_RENDER_TARGETS);
    Pegasus::Utils::Memset32(gDXState.mDispatchedTargets, 0, sizeof(gDXState.mDispatchedTargets));

    gDXState.mTargetsCount = renderTargetNum;

    for (int i = 0; i < renderTargetNum; ++i)
    {
        Pegasus::Render::DXRenderTargetGPUData* rtGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget[i]->GetInternalData());
        gDXState.mDispatchedTargets[i] = rtGpuData->mRenderTarget;
        
    }
    ID3D11DepthStencilView* d3dDepthStencil = nullptr;

    if (depthStencil != nullptr)
    {
        Pegasus::Render::DXDepthStencilGPUData* depthStencilGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXDepthStencilGPUData, depthStencil->GetInternalData());
        d3dDepthStencil = depthStencilGpuData->mDepthView;
    }

    gDXState.mDispatchedDepth = d3dDepthStencil;
    deviceContext->OMSetRenderTargets(
        renderTargetNum,
        gDXState.mDispatchedTargets,
        d3dDepthStencil
    );
}

void Pegasus::Render::SetComputeOutput(BufferRef buffer, int slot)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    PG_ASSERT(slot < MAX_UAV_SLOT_COUNT);
    Pegasus::Render::DXBufferGPUData* gpuBuffer = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXBufferGPUData, buffer->GetInternalData());
    if (gDXState.mComputeOutputs[slot] == nullptr) ++gDXState.mComputeOutputsCount;
    gDXState.mComputeOutputs[slot] = gpuBuffer->mUav;
    gDXState.mComputeOutputsDirty = true;
}

void Pegasus::Render::SetComputeOutput(VolumeTextureRef buffer, int slot)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    PG_ASSERT(slot < MAX_UAV_SLOT_COUNT);
    Pegasus::Render::DXTextureGPUData3d* texData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData3d, buffer->GetInternalData());
    if (gDXState.mComputeOutputs[slot] == nullptr) ++gDXState.mComputeOutputsCount;
    gDXState.mComputeOutputs[slot] = texData->mUav;
    gDXState.mComputeOutputsDirty = true;
}

void Pegasus::Render::SetComputeOutput(RenderTargetRef renderTarget, int slot)
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    PG_ASSERT(slot < MAX_UAV_SLOT_COUNT);
    Pegasus::Render::DXRenderTargetGPUData* texData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget->GetInternalData());
    if (gDXState.mComputeOutputs[slot] == nullptr) ++gDXState.mComputeOutputsCount;
    gDXState.mComputeOutputs[slot] = texData->mTextureView.mUav;
    gDXState.mComputeOutputsDirty = true;
}

void Pegasus::Render::UnbindComputeOutputs()
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();   
    
    unsigned int initialCounts[MAX_UAV_SLOT_COUNT];
    Pegasus::Utils::Memset32(initialCounts, 0, sizeof(initialCounts));
    Pegasus::Utils::Memset32(gDXState.mComputeOutputs, 0x0, sizeof(gDXState.mComputeOutputs));
    gDXState.mComputeOutputsDirty = false;
    gDXState.mComputeOutputsCount = 0;
    deviceContext->CSSetUnorderedAccessViews(0, MAX_UAV_SLOT_COUNT, gDXState.mComputeOutputs, initialCounts);
}

// ---------------------------------------------------------------------------

void Pegasus::Render::UnbindRenderTargets()
{
    DXRenderContext * ctx = DXRenderContext::GetBindedContext();
    ID3D11DeviceContext * deviceContext = ctx->GetD3D();
    ID3D11RenderTargetView* nullTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    Pegasus::Utils::Memset32(nullTargets, 0x0, sizeof(nullTargets));
    deviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullTargets, nullptr);
}

// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchDefaultRenderTarget()
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

	gDXState.mDispatchedTargets[0] = rt;
	gDXState.mDispatchedDepth = depthStencil;
    gDXState.mTargetsCount = 1;
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
        for (int i = 0; i < gDXState.mTargetsCount; ++i)
        {
            deviceContext->ClearRenderTargetView(
		    	gDXState.mDispatchedTargets[i],
                col.rgba
            );
        }
    }

	if (depth)
	{
        deviceContext->ClearDepthStencilView(
            gDXState.mDispatchedDepth ? gDXState.mDispatchedDepth : dsc,
            D3D11_CLEAR_DEPTH,
            gDXState.mDepthClearVal,
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
void Pegasus::Render::SetRasterizerState(const RasterizerStateRef& state)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    PG_ASSERT(state->GetInternalData() != nullptr && state->GetInternalDataAux() != nullptr);
    ID3D11RasterizerState* r = static_cast<ID3D11RasterizerState*>(state->GetInternalData());
    ID3D11DepthStencilState * d = static_cast<ID3D11DepthStencilState*>(state->GetInternalDataAux());
    context->RSSetState(r);
    context->OMSetDepthStencilState(d, 0);

    
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBLENDINGSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetBlendingState(const Pegasus::Render::BlendingStateRef blendingState)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11BlendState* d3dState = static_cast<ID3D11BlendState*>(blendingState->GetInternalData());
    context->OMSetBlendState(d3dState, NULL, 0xffffffff);
}



///////////////////////////////////////////////////////////////////////////////
/////////////   SETDEPTHCLEARVALUE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetDepthClearValue(float d)
{
    gDXState.mDepthClearVal = d;
}


///////////////////////////////////////////////////////////////////////////////
/////////////   SETPRIMITIVEMODE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetPrimitiveMode(Pegasus::Render::PrimitiveMode mode)
{
    gDXState.mPrimitiveMode = mode;
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DRAW FUNCTION IMPLEMENTATION      /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void DrawInternal(unsigned int instanceCount)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    if (gDXState.mDispatchedMeshGpuData == nullptr)
    {
        PG_LOG('ERR_', "A mesh must be set properly before calling draw!.");
        return;
    }
    Pegasus::Render::DXMeshGPUData* mesh = gDXState.mDispatchedMeshGpuData;

    if (gDXState.mPrimitiveMode == Pegasus::Render::PRIMITIVE_AUTOMATIC)
    {        
        context->IASetPrimitiveTopology(mesh->mTopology);
    }
    else
    {
        PG_ASSERT(gDXState.mPrimitiveMode >= 0 && gDXState.mPrimitiveMode < Pegasus::Render::PRIMITIVE_COUNT);
        context->IASetPrimitiveTopology(gPrimitiveModeTranslation[gDXState.mPrimitiveMode]);
    }

    if (mesh->mIsIndexed)
    {
        if (mesh->mIsIndirect)
        {
            context->DrawIndexedInstancedIndirect(
                mesh->mIndirectDrawStream.mBuffer,
                0
            );
        }   
        else
        {
            if (instanceCount > 0)
            {
                context->DrawIndexedInstanced(
                    mesh->mIndexCount,
                    instanceCount,
                    0,
                    0,
                    0
                );
            }
            else
            {
                context->DrawIndexed(
                    mesh->mIndexCount,
                    0,
                    0
                );
            }
        }
    }
    else
    {
        PG_ASSERTSTR(!mesh->mIsIndexed, "Pegasus only supports indirect draw indexed. Setting a mesh as indirect, but not making it indexed.");
        if (instanceCount > 0)
        {
            context->DrawInstanced(
                mesh->mVertexCount,
                instanceCount,
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
}

void Pegasus::Render::Draw()
{
    DrawInternal(0);
}

void Pegasus::Render::DrawInstanced(unsigned int instanceCount)
{
    if (instanceCount > 0)
    {
        DrawInternal(instanceCount);
    }
}

void Pegasus::Render::Dispatch(unsigned int x, unsigned int y, unsigned int z)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    if (gDXState.mComputeOutputsDirty)
    {
        unsigned int initialCounts[MAX_UAV_SLOT_COUNT];
        Utils::Memset32(initialCounts, 0, sizeof(initialCounts));
        context->CSSetUnorderedAccessViews(0, gDXState.mComputeOutputsCount, gDXState.mComputeOutputs, initialCounts);
        gDXState.mComputeOutputsDirty = false;
    }

    context->Dispatch(x, y, z);
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   GET UNIFORM FUNCTION IMPLEMENTATIONS    ///////////////////////
///////////////////////////////////////////////////////////////////////////////
bool UpdateUniformLocation(Pegasus::Render::DXProgramGPUData* programGPUData, const char * name, Pegasus::Render::Uniform& outputUniform)
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

void Pegasus::Render::DXInitBufferData(Pegasus::Render::DXBufferGPUData& outBuffer)
{
    Pegasus::Utils::Memset8(&outBuffer.mDesc   ,  0x0, sizeof(outBuffer.mDesc));
    Pegasus::Utils::Memset8(&outBuffer.mUavDesc,  0x0, sizeof(outBuffer.mUavDesc));
    Pegasus::Utils::Memset8(&outBuffer.mSrvDesc,  0x0, sizeof(outBuffer.mSrvDesc));
}

void Pegasus::Render::DXCreateBuffer(
    ID3D11Device * device,
    int bufferSize,
    int elementCount,
    bool isDynamic,
    void* initData,
    D3D11_BIND_FLAG bindFlags,
    Pegasus::Render::DXBufferGPUData& outBuffer,
    UINT extraMiscFlags)
{
    
    const bool isCompute = (bindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0;
    const bool isIndex = (bindFlags & D3D11_BIND_INDEX_BUFFER) != 0;
    const bool isStructured = (extraMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) != 0;
    PG_ASSERTSTR((isStructured && ((bufferSize % elementCount) == 0)) || !isStructured, "Structured buffer byte size is not a multiple of its stride.");

    D3D11_BUFFER_DESC& desc = outBuffer.mDesc;
    desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    desc.ByteWidth = bufferSize;
    desc.BindFlags |= bindFlags;

    desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.MiscFlags = ((isCompute && !isIndex) ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0) | extraMiscFlags;
    desc.StructureByteStride = isStructured ? (bufferSize / elementCount) : 0;

    D3D11_SUBRESOURCE_DATA srd;
    srd.pSysMem = initData;
    srd.SysMemPitch = 0;
    srd.SysMemSlicePitch = 0;

    VALID_DECLARE(device->CreateBuffer(&desc, isCompute ? nullptr : (initData == nullptr ? nullptr : &srd), &outBuffer.mBuffer));

    if (isCompute && outBuffer.mBuffer != nullptr)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc = outBuffer.mUavDesc;
        uavDesc.Format =  isIndex ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_TYPELESS;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = (UINT)desc.ByteWidth/4;
        uavDesc.Buffer.Flags = isIndex ? 0 : D3D11_BUFFER_UAV_FLAG_RAW;
        VALID_DECLARE(device->CreateUnorderedAccessView(outBuffer.mBuffer, &uavDesc, &outBuffer.mUav));
        
    }

    bool isSrv = (bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0;
    if (isSrv && outBuffer.mBuffer != nullptr)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = outBuffer.mSrvDesc;
        if (isIndex)
        {
            srvDesc.Format = DXGI_FORMAT_R16_UINT;
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements = (UINT)desc.ByteWidth/2;
            srvDesc.BufferEx.Flags = 0;
        }
        else if (isStructured)
        {
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.ElementWidth = elementCount;
        }
        else
        {
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;;
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements = (UINT)desc.ByteWidth/4;
            srvDesc.BufferEx.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        }
        VALID_DECLARE(device->CreateShaderResourceView(outBuffer.mBuffer, &srvDesc, &outBuffer.mSrv));
    }
}

Pegasus::Render::BufferRef Pegasus::Render::CreateUniformBuffer(int size)
{
    Pegasus::Render::Buffer* b = RENDER_NEW(Pegasus::Render::Buffer);
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXBufferGPUData* bufferGpuData = RENDER_NEW_GPU_DATA(Pegasus::Render::DXBufferGPUData);
    Pegasus::Render::DXInitBufferData(*bufferGpuData);

    Pegasus::Render::DXCreateBuffer(
        device,
        size,
        0, /*not at array, so no need for element count*/
        true, /*isDynamic*/
        nullptr, /*init data*/
        (D3D11_BIND_FLAG)D3D11_BIND_CONSTANT_BUFFER,
        *bufferGpuData);

    BufferConfig bc;
    bc.mSize = size;
    b->SetConfig(bc);
    b->SetInternalData(bufferGpuData);
    return b;
}

Pegasus::Render::BufferRef Pegasus::Render::CreateComputeBuffer(int bufferSize, int elementCount, bool makeUniformBuffer)
{
    Pegasus::Render::Buffer* b = RENDER_NEW(Pegasus::Render::Buffer);
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXBufferGPUData* bufferGpuData = RENDER_NEW_GPU_DATA(Pegasus::Render::DXBufferGPUData);
    Pegasus::Render::DXInitBufferData(*bufferGpuData);

    Pegasus::Render::DXCreateBuffer(
        device,
        bufferSize,
        elementCount, /*not at array, so no need for element count*/
        false, /*isDynamic*/
        nullptr, /*init data*/
        (D3D11_BIND_FLAG)((makeUniformBuffer ? D3D11_BIND_CONSTANT_BUFFER : 0) | (D3D11_BIND_UNORDERED_ACCESS) | (D3D11_BIND_SHADER_RESOURCE)),
        *bufferGpuData);

    BufferConfig bc;
    bc.mSize = bufferSize;
    b->SetConfig(bc);
    b->SetInternalData(bufferGpuData);
    return b;
}

Pegasus::Render::BufferRef Pegasus::Render::CreateStructuredReadBuffer(int bufferSize, int elementCount)
{
    Pegasus::Render::Buffer* b = RENDER_NEW(Pegasus::Render::Buffer);
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXBufferGPUData* bufferGpuData = RENDER_NEW_GPU_DATA(Pegasus::Render::DXBufferGPUData);
    Pegasus::Render::DXInitBufferData(*bufferGpuData);

    Pegasus::Render::DXCreateBuffer(
        device,
        bufferSize,
        elementCount, /*not at array, so no need for element count*/
        true, /*isDynamic*/
        nullptr, /*init data*/
        (D3D11_BIND_FLAG)D3D11_BIND_SHADER_RESOURCE,
        *bufferGpuData,
        D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

    BufferConfig bc;
    bc.mSize = bufferSize;
    b->SetConfig(bc);
    b->SetInternalData(bufferGpuData);
    return b;
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::BufferConfig>::~BasicResource()
{
    if (GetInternalData() != nullptr)
    {
        Pegasus::Render::DXBufferGPUData * gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXBufferGPUData, GetInternalData());
        gpuData->mBuffer = nullptr;
        gpuData->mUav = nullptr;
        gpuData->mSrv = nullptr;
        PG_DELETE(Pegasus::Memory::GetRenderAllocator(), gpuData);
    }     
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

Pegasus::Render::RasterizerStateRef Pegasus::Render::CreateRasterizerState(const Pegasus::Render::RasterizerConfig& config)
{
    Pegasus::Render::RasterizerState* rasterizerState = RENDER_NEW(Pegasus::Render::RasterizerState);
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    rasterizerState->SetConfig(config);

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
    rasterizerState->SetInternalData(static_cast<void*>(rasterState));

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
    rasterizerState->SetInternalDataAux( static_cast<void*>(depthStencilState) );
    return rasterizerState;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

Pegasus::Render::BlendingStateRef Pegasus::Render::CreateBlendingState(const Pegasus::Render::BlendingConfig& config)
{
    Pegasus::Render::BlendingState* blendingState = RENDER_NEW(Pegasus::Render::BlendingState);
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
        D3D11_BLEND_ONE,
        D3D11_BLEND_SRC_ALPHA,
        D3D11_BLEND_INV_SRC_ALPHA,
        D3D11_BLEND_DEST_ALPHA,
        D3D11_BLEND_INV_DEST_ALPHA
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
    blendingState->SetInternalData(static_cast<void*>(d3dBlendState));
    blendingState->SetConfig(config);
    return blendingState;
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::RasterizerConfig>::~BasicResource()
{
    PG_ASSERT(GetInternalData() != nullptr && GetInternalDataAux() != nullptr);
    ID3D11RasterizerState* r = static_cast<ID3D11RasterizerState*>(GetInternalData());
    ID3D11DepthStencilState * d = static_cast<ID3D11DepthStencilState*>(GetInternalDataAux());

    r->Release();
    d->Release();
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
template<>
Pegasus::Render::BasicResource<Pegasus::Render::BlendingConfig>::~BasicResource()
{
    PG_ASSERT(GetInternalData() != nullptr);
    ID3D11BlendState* d3dBlendState = static_cast<ID3D11BlendState*>(GetInternalData());
    d3dBlendState->Release();
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBUFFER IMPLEMENTATION                ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetBuffer(Pegasus::Render::BufferRef& dstBuffer, const void * src, int size, int offset)
{
    Pegasus::Render::DXBufferGPUData* bufferGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXBufferGPUData, dstBuffer->GetInternalData());

    ID3D11Buffer* d3dBuffer = static_cast<ID3D11Buffer*>(bufferGpuData->mBuffer);
    size = size == -1 ? dstBuffer->GetConfig().mSize : size;
    int actualSize = size + offset;
    PG_ASSERT(d3dBuffer != nullptr);
    PG_ASSERT(actualSize <= dstBuffer->GetConfig().mSize);
    
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    D3D11_MAPPED_SUBRESOURCE srd;
    if (context->Map(d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &srd) == S_OK)
    {
        Pegasus::Utils::Memcpy(srd.pData, static_cast<const char*>(src) + offset, size);
        context->Unmap(d3dBuffer, 0);
    }
    else
    {
        PG_FAILSTR("Map of subresource failed");
    }
}

// ---------------------------------------------------------------------------

void Pegasus::Render::UnbindComputeResources()
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11ShaderResourceView* nullResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
    Pegasus::Utils::Memset32(nullResources, 0x0, sizeof(nullResources));
    context->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullResources);
}

void Pegasus::Render::UnbindPixelResources()
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11ShaderResourceView* nullResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
    Pegasus::Utils::Memset32(nullResources, 0x0, sizeof(nullResources));
    context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullResources);
}

void Pegasus::Render::UnbindVertexResources()
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    ID3D11ShaderResourceView* nullResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
    Pegasus::Utils::Memset32(nullResources, 0x0, sizeof(nullResources));
    context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullResources);
}

static bool InternalSetShaderResource(Pegasus::Render::Uniform& u, ID3D11ShaderResourceView* srv)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    PG_ASSERT(srv != nullptr);
    
    Pegasus::Render::DXProgramGPUData * programData = gDXState.mDispatchedShader;
	if (programData != nullptr && programData->mProgramValid && ProcessUpdateUniform(u,programData))
    {
		if (u.mInternalIndex < 0 || u.mInternalIndex >= programData->mReflectionDataCount)
        {
            PG_LOG('ERR_', "Fatal error when setting uniform %s. Does this uniform corresponds to the program?", u.mName);
            return false;
        }
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
    return InternalSetShaderResource(u, texGpuData->mSrv);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformBuffer(Pegasus::Render::Uniform& u, const BufferRef& buffer)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    DXProgramGPUData * programData = gDXState.mDispatchedShader;
	if (programData != nullptr && programData->mProgramValid && ProcessUpdateUniform(u,programData))
    {
		if (u.mInternalIndex < 0 || u.mInternalIndex >= programData->mReflectionDataCount)
        {
            PG_LOG('ERR_', "Fatal error when setting uniform %s. Does this uniform corresponds to the program?", u.mName);
            return false;
        }
        Pegasus::Render::DXBufferGPUData* gpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXBufferGPUData, buffer->GetInternalData());
        ID3D11Buffer* d3dBuffer = gpuData->mBuffer;
        PG_ASSERT(d3dBuffer != nullptr);
        Pegasus::Render::DXProgramGPUData::UniformReflectionData& reflectionData = programData->mReflectionData[u.mInternalIndex];
		for (int s = 0; s < reflectionData.mStageCount; ++s)
        {
            Pegasus::Render::DXProgramGPUData::UniformReflectionData::StageBinding& binding = reflectionData.mStageBindings[s];
            if (binding.mSize > buffer->GetConfig().mSize)
            {
                PG_LOG('ERR_', "Size of cbuffer too small. Target size of \"%s\" must be %d bytes, instead expecting %d bytes", u.mName, binding.mSize, buffer->GetConfig().mSize);
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

bool Pegasus::Render::SetUniformTextureRenderTarget(Pegasus::Render::Uniform& u, const RenderTargetRef& renderTarget)
{
    Pegasus::Render::DXRenderTargetGPUData * renderTargetGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget->GetInternalData());
    return InternalSetShaderResource(u, renderTargetGpuData->mTextureView.mSrv);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformDepth(Pegasus::Render::Uniform& u, const DepthStencilRef& depth)
{
    Pegasus::Render::DXDepthStencilGPUData * depthGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXDepthStencilGPUData, depth->GetInternalData());
    return InternalSetShaderResource(u, depthGpuData->mSrvDepth);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformStencil(Pegasus::Render::Uniform& u, const DepthStencilRef& stencil)
{
    Pegasus::Render::DXDepthStencilGPUData * stencilGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXDepthStencilGPUData, stencil->GetInternalData());
    return InternalSetShaderResource(u, stencilGpuData->mSrvStencil);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformBufferResource(Pegasus::Render::Uniform& u, const BufferRef& buffer)
{
    Pegasus::Render::DXBufferGPUData * bufferGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXBufferGPUData, buffer->GetInternalData());
    return InternalSetShaderResource(u, bufferGpuData->mSrv);
}

bool Pegasus::Render::SetUniformVolume(Pegasus::Render::Uniform& u, const VolumeTextureRef& volume)
{
    Pegasus::Render::DXTextureGPUData3d * gpuVol = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData3d, volume->GetInternalData());
    return InternalSetShaderResource(u, gpuVol->mSrv);
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniformCubeMap(Pegasus::Render::Uniform& u, CubeMapRef& cubeMap)
{
    //todo: implement
    return false;
}

// ---------------------------------------------------------------------------
void Pegasus::Render::CleanInternalState()
{
    gDXState.mTargetsCount = 0;
    gDXState.mDispatchedMeshGpuData = nullptr;
    gDXState.mDispatchedShader = nullptr;
    gDXState.mDispatchedProgramVersion = 0;
    gDXState.mDispatchedMeshVersion = 0;
    gDXState.mDispatchedMeshGpuData = nullptr;
    gDXState.mClearColorValue = Pegasus::Math::ColorRGBA(0.0f,0.0f,0.0f,0.0f);
    gDXState.mDepthClearVal = 1.0f;
    gDXState.mPrimitiveMode = Pegasus::Render::PRIMITIVE_AUTOMATIC;
    Utils::Memset32(gDXState.mComputeOutputs, 0, sizeof(gDXState.mComputeOutputs));
    gDXState.mComputeOutputsCount = 0;
    gDXState.mComputeOutputsDirty = false;

    Utils::Memset32(gDXState.mDispatchedTargets, 0, sizeof(gDXState.mDispatchedTargets));
    gDXState.mDispatchedDepth = nullptr;
}

void Pegasus::Render::BeginMarker(const char* marker)
{
#if PEGASUS_GPU_DEBUG
    Pegasus::Render::DXRenderContext* bindedContext = Pegasus::Render::DXRenderContext::GetBindedContext();
    if (bindedContext != nullptr)
    {
        bindedContext->BeginMarker(marker);
    }
#endif
}

void Pegasus::Render::EndMarker()
{
#if PEGASUS_GPU_DEBUG
    Pegasus::Render::DXRenderContext* bindedContext = Pegasus::Render::DXRenderContext::GetBindedContext();
    if (bindedContext != nullptr)
    {
        bindedContext->EndMarker();
    }
#endif
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
