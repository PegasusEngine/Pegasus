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
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"

//////////////////        GLOBALS CODE BLOCK     //////////////////////////////
//         All globals holding state data are declared on this block       ////
///////////////////////////////////////////////////////////////////////////////


__declspec( thread )
struct DXState
{
    Pegasus::Render::DXProgramGPUData * mDispatchedShader;
    Pegasus::Render::DXMeshGPUData    * mDispatchedMeshGpuData;
} gDXState = { nullptr, nullptr };

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DISPATCH FUNCTIONS IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void Pegasus::Render::Dispatch (Pegasus::Shader::ProgramLinkageInOut program)
{
    bool updated = false;
    Pegasus::Graph::NodeGPUData * nodeGpuData = program->GetUpdatedData(updated)->GetNodeGPUData();
    Pegasus::Render::DXProgramGPUData * shaderGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXProgramGPUData, nodeGpuData);

    if (gDXState.mDispatchedShader != shaderGpuData)
    {
        ID3D11DeviceContext * context;
        ID3D11Device * device;
        Pegasus::Render::GetDeviceAndContext(&device, &context);
        gDXState.mDispatchedShader = shaderGpuData;
        context->VSSetShader(shaderGpuData->mVertex, nullptr, 0);
        context->PSSetShader(shaderGpuData->mPixel, nullptr, 0);
        context->DSSetShader(shaderGpuData->mDomain, nullptr, 0);
        context->HSSetShader(shaderGpuData->mHull, nullptr, 0);
        context->GSSetShader(shaderGpuData->mGeometry, nullptr, 0);
        context->CSSetShader(shaderGpuData->mCompute, nullptr, 0);
    }
}

// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch (Pegasus::Mesh::MeshInOut mesh)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    Pegasus::Graph::NodeGPUData * nodeGpuData = mesh->GetUpdatedMeshData()->GetNodeGPUData();
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
        VALID_DECLARE(
            device->CreateInputLayout(
                meshGpuData->mInputElementsDesc,
				meshGpuData->mInputElementsCount,
                programGpuData->mInputLayoutBlob->GetBufferPointer(),
                programGpuData->mInputLayoutBlob->GetBufferSize(),
                &inputLayoutEntry->mInputLayout
            )
        );
        inputLayoutEntry->mProgramGuid = programGpuData->mProgramGuid;
        inputLayoutEntry->mProgramVersion = programGpuData->mProgramVersion;
    }

    PG_ASSERT(inputLayoutEntry != nullptr);
    context->IASetInputLayout(inputLayoutEntry->mInputLayout);
   
}


// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch(Pegasus::Render::RenderTarget& renderTarget, const Viewport& viewport, int renderTargetSlot)
{
}


// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchNullRenderTarget()
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchDefaultRenderTarget(const Pegasus::Render::Viewport& viewport)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   Clear Functions                         ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Clear(bool color, bool depth, bool stencil)
{
}

void Pegasus::Render::SetClearColorValue(const Pegasus::Math::ColorRGBA& color)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETRASTERIZERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRasterizerState(const RasterizerState& rasterState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBLENDINGSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetBlendingState(const Pegasus::Render::BlendingState& blendingState)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::SetDepthClearValue(float d)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch(Pegasus::Render::RenderTarget& renderTarget)
{
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DRAW FUNCTION IMPLEMENTATION      /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Draw()
{
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   GET UNIFORM FUNCTION IMPLEMENTATIONS    ///////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Pegasus::Render::GetUniformLocation(Pegasus::Shader::ProgramLinkageInOut program, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEUNIFORMBUFFER IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateUniformBuffer(int size, Pegasus::Render::Buffer& outputBuffer)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateRasterizerState(Pegasus::Render::RasterizerConfig& config, Pegasus::Render::RasterizerState& rasterizerState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateBlendingState(Pegasus::Render::BlendingConfig& config, Pegasus::Render::BlendingState& blendingState)
{
}
///////////////////////////////////////////////////////////////////////////////
/////////////   DELETERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRasterizerState(Pegasus::Render::RasterizerState& state)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteBlendingState(Pegasus::Render::BlendingState& blendingState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBUFFER IMPLEMENTATION                ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetBuffer(Pegasus::Render::Buffer& dstBuffer, void * src, int size, int offset)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::DeleteBuffer(Pegasus::Render::Buffer& buffer)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETUNIFORMS IMPLEMENTATIONS             ///////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, float value)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, Pegasus::Texture::TextureInOut texture)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, const Buffer& buffer)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, const RenderTarget& renderTarget)
{
    return false;
}


#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
