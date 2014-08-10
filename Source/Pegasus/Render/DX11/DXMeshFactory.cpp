/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXMeshFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 mesh factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Mesh/MeshData.h"
#include "Pegasus/Mesh/MeshConfiguration.h"
#include "Pegasus/Mesh/MeshInputLayout.h"

#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Memcpy.h"

#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"

#include <dxgiformat.h>


//translation table for DX11 types from Pegasus::Mesh
const char * gSemanticTranslations[Pegasus::Mesh::MeshInputLayout::SEMANTIC_COUNT] =
{
    "POSITION",  //! Position, used by pegasus to know where the mesh position is determined
    "NORMAL",    //! Normal, used by pegasus to know where the normal is located
    "TANGENT",   //! Tangent, used by pegasus to know where the tangent is located
    "BITANGENT", //! Bitangent, used by pegasus to know where the bitangent is located
    "TEXCOORD",        //! UV coordinates, used by pegasus to know where the UVs are located 
    "COLOR",     //! Color attribute of mesh, used by pegasus to know where color is located
    "BLEND_INDEX",  //! Blend index, used by pegasus to know where the blend index is located
    "BLEND_WEIGHT", //! Blend weight, used by pegasus to know where the blend weight is located
    "USER_GENERIC" //! user generic, used for any generic custom attribute type
};

static const DXGI_FORMAT DXGI_INVALID_FMT = static_cast<DXGI_FORMAT>(-1);
const DXGI_FORMAT gVertexFormatTranslations [Pegasus::Mesh::MeshInputLayout::ATTRTYPE_COUNT][4] =
{
       //           0                      1                         2                            3
       /* FLOAT */ {DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
       /* INT   */ {DXGI_FORMAT_R32_SINT,  DXGI_FORMAT_R32G32_SINT,  DXGI_FORMAT_R32G32B32_SINT,  DXGI_FORMAT_R32G32B32A32_SINT     },
       /* UNT   */ {DXGI_FORMAT_R32_UINT,  DXGI_FORMAT_R32G32_UINT,  DXGI_FORMAT_R32G32B32_UINT,  DXGI_FORMAT_R32G32B32A32_UINT     },
       /* SHORT */ {DXGI_FORMAT_R16_SINT,  DXGI_FORMAT_R16G16_SINT,  DXGI_INVALID_FMT          ,  DXGI_FORMAT_R16G16B16A16_SINT     },
       /* SHORT */ {DXGI_FORMAT_R16_UINT,  DXGI_FORMAT_R16G16_UINT,  DXGI_INVALID_FMT          ,  DXGI_FORMAT_R16G16B16A16_UINT     },
       /* BOOL  */ {DXGI_INVALID_FMT,      DXGI_INVALID_FMT,         DXGI_INVALID_FMT          ,  DXGI_INVALID_FMT}
};


class DXMeshFactory : public Pegasus::Mesh::IMeshFactory
{
public:
    DXMeshFactory() : mAllocator(nullptr){}
    virtual ~DXMeshFactory(){}
    
    
    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);
    
    virtual void GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData);
    
    virtual void DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData);

    virtual Pegasus::Alloc::IAllocator * GetAllocator() { return mAllocator; }
    
private:
    Pegasus::Render::DXMeshGPUData* GetOrAllocateGPUData(Pegasus::Mesh::MeshData * nodeData);

    Pegasus::Alloc::IAllocator* mAllocator;
};

Pegasus::Render::DXMeshGPUData* DXMeshFactory::GetOrAllocateGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    Pegasus::Graph::NodeGPUData* nodeGpuData = nodeData->GetNodeGPUData();
    if (nodeGpuData == nullptr)
    {
        Pegasus::Render::DXMeshGPUData* meshGpuData = PG_NEW(
            mAllocator,
            -1,
            "DXMeshGPUData",
            Pegasus::Alloc::PG_MEM_PERM
        ) Pegasus::Render::DXMeshGPUData;

        meshGpuData->mInputElementsCount = 0;
        meshGpuData->mInputLayoutTableCount = 0;
        meshGpuData->mInputLayoutTableCapacity = Pegasus::Render::DXMeshGPUData::INPUT_LAYOUT_TABLE_INCREMENT;
        meshGpuData->mInputLayoutTable = PG_NEW_ARRAY(
            mAllocator,
            -1,
            "MeshGPUData inputLayoutTable",
            Pegasus::Alloc::PG_MEM_TEMP,
            Pegasus::Render::DXMeshGPUData::InputLayoutEntry,
            meshGpuData->mInputLayoutTableCapacity
        );
        meshGpuData->mIsIndexed = false;
        meshGpuData->mVertexCount = 0;
        meshGpuData->mIndexCount = 0;


        Pegasus::Utils::Memset8(&meshGpuData->mVertexBufferDesc, 0x0, sizeof(meshGpuData->mVertexBufferDesc));
        Pegasus::Utils::Memset8(&meshGpuData->mIndexBufferDesc, 0x0, sizeof(meshGpuData->mIndexBufferDesc));
        nodeGpuData = reinterpret_cast<Pegasus::Graph::NodeGPUData*>(meshGpuData);
        nodeData->SetNodeGPUData(nodeGpuData);
    };
    
    return PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeGpuData);

}

void DXMeshFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
    mAllocator = allocator;
}

void DXMeshFactory::GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    const Pegasus::Mesh::MeshConfiguration& configuration = nodeData->GetConfiguration();
    const Pegasus::Mesh::MeshInputLayout*   meshInputLayout = configuration.GetInputLayout();
    Pegasus::Render::DXMeshGPUData*   meshGpuData = GetOrAllocateGPUData(nodeData);
    meshGpuData->mIsIndexed = configuration.GetIsIndexed();

    int vertexCount = nodeData->GetVertexCount();
    meshGpuData->mVertexCount = vertexCount;

    for (int streamIndex = 0; streamIndex < MESH_MAX_STREAMS; ++streamIndex)
    {
        if (nodeData->GetStreamStride(streamIndex) > 0)
        {
            D3D11_BUFFER_DESC& streamDesc = meshGpuData->mVertexBufferDesc[streamIndex];
            CComPtr<ID3D11Buffer>& bufferPtr = meshGpuData->mVertexBuffer[streamIndex];
            int streamByteSize = nodeData->GetStreamStride(streamIndex) * vertexCount;

            //has the size changed? or are we trying to update the contents of a non dynamic resource?
            if (bufferPtr != nullptr && (streamByteSize != streamDesc.ByteWidth || streamDesc.Usage == D3D11_USAGE_DEFAULT))
            {
                //erase this buffer
                bufferPtr = nullptr;
            }

            if (bufferPtr == nullptr)
            {
                streamDesc.Usage = configuration.GetIsDynamic() ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                streamDesc.ByteWidth = streamByteSize;
                streamDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                streamDesc.CPUAccessFlags = configuration.GetIsDynamic() ? D3D11_CPU_ACCESS_WRITE : 0;
                streamDesc.MiscFlags = 0;
            
                D3D11_SUBRESOURCE_DATA initData;
                initData.pSysMem = nodeData->GetStream<void>(streamIndex);
                initData.SysMemPitch = 0;
                initData.SysMemSlicePitch = 0;
            
                VALID_DECLARE(device->CreateBuffer(&streamDesc, &initData, &bufferPtr));
            }
            else
            {
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                if (context->Map(bufferPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
                {
                    PG_ASSERTSTR(mappedResource.pData != nullptr,"map returned a null pointer of data!");
                    Pegasus::Utils::Memcpy(mappedResource.pData, nodeData->GetStream<void>(streamIndex), streamByteSize);
                    context->Unmap(bufferPtr, 0);
                }
                else
                {
                    PG_FAILSTR("GPU Map failed!");
                }
            }
        }
    }

    if (configuration.GetIsIndexed())
    {
        if (meshGpuData->mIndexBuffer != nullptr && meshGpuData->mIndexBufferDesc.ByteWidth != nodeData->GetIndexCount() * sizeof(unsigned short))
        {
            meshGpuData->mIndexBuffer = nullptr;
        }

        if (meshGpuData->mIndexBuffer == nullptr)
        {
            meshGpuData->mIndexCount = nodeData->GetIndexCount();
            D3D11_BUFFER_DESC& bufferDesc = meshGpuData->mIndexBufferDesc;
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.ByteWidth = nodeData->GetIndexCount() * sizeof(unsigned short);
            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDesc.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA initData;
	    	initData.pSysMem = static_cast<void*>(nodeData->GetIndexBuffer());
            initData.SysMemPitch = 0;
            initData.SysMemSlicePitch = 0;
            VALID_DECLARE(device->CreateBuffer(&bufferDesc, &initData, &meshGpuData->mIndexBuffer));
        }
        else
        {
            PG_ASSERT(
                meshGpuData->mIndexBuffer != nullptr &&
                meshGpuData->mIndexBufferDesc.Usage == D3D11_USAGE_DYNAMIC &&
                meshGpuData->mIndexBufferDesc.ByteWidth == nodeData->GetIndexCount() * sizeof(unsigned short)
            );

            D3D11_MAPPED_SUBRESOURCE mappedResource;
            if (context->Map(meshGpuData->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
            {
                PG_ASSERTSTR(mappedResource.pData != nullptr, "map returned a null pointer of data!");
                Pegasus::Utils::Memcpy(mappedResource.pData, nodeData->GetIndexBuffer(), nodeData->GetIndexCount() * sizeof(unsigned short));
                context->Unmap(meshGpuData->mIndexBuffer, 0);
            }
            else
            {
                PG_FAILSTR("GPU Map failed!");
            }
        }
    }

    //reset all the existing input layouts
    for (int i = 0; i < meshGpuData->mInputLayoutTableCount; ++i)
    {
        meshGpuData->mInputLayoutTable[i].mInputLayout = nullptr; //delete
    }
    meshGpuData->mInputLayoutTableCount = 0;

    //translate mesh configuration into input layout structure
    int attributeCount = meshInputLayout->GetAttributeCount();
    PG_ASSERTSTR(attributeCount < Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC, "Exceeding the maximum number of input layout elements. Truncating the rest");
    //sanity check for attribute count
    attributeCount = attributeCount < Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC ? attributeCount : Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC - 1;
    
    //clear the previous translated description
    meshGpuData->mInputElementsCount = 0;
    for (int i = 0; i < attributeCount; ++i)
    {
        const Pegasus::Mesh::MeshInputLayout::AttrDesc& attr = meshInputLayout->GetAttributeDesc(i);
        PG_ASSERT(
            attr.mType < Pegasus::Mesh::MeshInputLayout::ATTRTYPE_COUNT &&
            attr.mAttributeTypeCount > 0 && attr.mAttributeTypeCount <= 4 && 
            attr.mSemanticIndex < Pegasus::Mesh::MeshInputLayout::SEMANTIC_COUNT
        );
        
        DXGI_FORMAT fmt = gVertexFormatTranslations[attr.mType][attr.mAttributeTypeCount - 1];
        const char * semanticStr = gSemanticTranslations[attr.mSemantic];
        
        D3D11_INPUT_ELEMENT_DESC& descStruct = meshGpuData->mInputElementsDesc[meshGpuData->mInputElementsCount++];
        descStruct.SemanticName = semanticStr;
        descStruct.SemanticIndex = attr.mSemanticIndex;
        descStruct.Format = fmt;
        descStruct.InputSlot = attr.mStreamIndex;
        descStruct.AlignedByteOffset = attr.mByteOffset;
        descStruct.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        descStruct.InstanceDataStepRate = 0;
    }

    nodeData->ValidateGPUData();
}

void DXMeshFactory::DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    Pegasus::Graph::NodeGPUData* nodeGpuData = nodeData->GetNodeGPUData();
    if (nodeGpuData != nullptr)
    {
        Pegasus::Render::DXMeshGPUData* meshGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXMeshGPUData, nodeGpuData);
        for (int i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            meshGpuData->mVertexBuffer[i] = nullptr;
        }
        meshGpuData->mIndexBuffer = nullptr;
        //reset all the existing input layouts
        for (int i = 0; i < meshGpuData->mInputLayoutTableCount; ++i)
        {
            meshGpuData->mInputLayoutTable[i].mInputLayout = nullptr; //delete
        }
        PG_DELETE_ARRAY(mAllocator, meshGpuData->mInputLayoutTable);
        PG_DELETE(mAllocator, meshGpuData);
        nodeData->SetNodeGPUData(nullptr);
    }
}

namespace Pegasus
{

namespace Render
{

DXMeshFactory gMeshFactory;

Mesh::IMeshFactory * GetRenderMeshFactory()
{
    return &gMeshFactory;
}

}

}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
