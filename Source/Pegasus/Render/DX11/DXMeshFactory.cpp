/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXMeshFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 mesh factory implementation

#if PEGASUS_GAPI_DX11

#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Mesh/MeshData.h"
#include "Pegasus/Mesh/MeshConfiguration.h"
#include "Pegasus/Mesh/MeshInputLayout.h"
#include "Pegasus/Graph/Node.h"

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

extern DXGI_FORMAT GetDxFormat(Pegasus::Core::Format format);

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

        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        meshGpuData->mIsIndexed = false;
        meshGpuData->mIsIndirect = false;
        meshGpuData->mVertexCount = 0;
        meshGpuData->mIndexCount = 0;

        for (unsigned i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            Pegasus::Render::DXInitBufferData(meshGpuData->mVertexStreams[i]);
            Pegasus::Render::Buffer* bufferWrapper = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "VertexStreamBuffer", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::Buffer(Pegasus::Memory::GetRenderAllocator()); 
            bufferWrapper->SetInternalData(&meshGpuData->mVertexStreams[i]);
            meshGpuData->mVertexBuffers[i] = bufferWrapper;
        }

        {
            Pegasus::Render::DXInitBufferData(meshGpuData->mIndirectDrawStream);
            Pegasus::Render::Buffer* bufferWrapper = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "IndirectDrawBufferStream", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::Buffer(Pegasus::Memory::GetRenderAllocator()); 
            bufferWrapper->SetInternalData(&meshGpuData->mIndirectDrawStream);
            meshGpuData->mDrawIndirectBuffer = bufferWrapper;
        }
        
        {
            Pegasus::Render::DXInitBufferData(meshGpuData->mIndexStream);
            Pegasus::Render::Buffer* bufferWrapper = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "IndexStreamBuffer", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Render::Buffer(Pegasus::Memory::GetRenderAllocator()); 
            bufferWrapper->SetInternalData(&meshGpuData->mIndexStream);
            meshGpuData->mIndexBuffer = bufferWrapper;
        }
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
    const bool isCompute = nodeData->GetMode() == Pegasus::Graph::Node::COMPUTE;
    
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);
    const Pegasus::Mesh::MeshConfiguration& configuration = nodeData->GetConfiguration();
    const Pegasus::Mesh::MeshInputLayout&   meshInputLayout = configuration.GetInputLayout();
    Pegasus::Render::DXMeshGPUData*   meshGpuData = GetOrAllocateGPUData(nodeData);
    meshGpuData->mIsIndexed = configuration.GetIsIndexed();
    meshGpuData->mIsIndirect = configuration.GetIsDrawIndirect();
    switch(configuration.GetMeshPrimitiveType())
    {    
    case Pegasus::Mesh::MeshConfiguration::TRIANGLE:
        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case Pegasus::Mesh::MeshConfiguration::TRIANGLE_STRIP:    
        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case Pegasus::Mesh::MeshConfiguration::LINE:
        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case Pegasus::Mesh::MeshConfiguration::LINE_STRIP:
        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case Pegasus::Mesh::MeshConfiguration::POINT:
        meshGpuData->mTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    default:
        PG_FAILSTR("Unsupported primitive topology. Defaulting to trianglelist.");    
    }

    int vertexCount = nodeData->GetVertexCount();
    meshGpuData->mVertexCount = vertexCount;
    PG_ASSERTSTR(vertexCount != 0, "Cannot pass 0 size vertex buffer. Forgot to call AllocVertices on meshData?");
    for (int streamIndex = 0; streamIndex < MESH_MAX_STREAMS; ++streamIndex)
    {
        if (nodeData->GetStreamStride(streamIndex) > 0)
        {
            Pegasus::Render::DXBufferGPUData& bufferData = meshGpuData->mVertexStreams[streamIndex];
            D3D11_BUFFER_DESC& streamDesc = bufferData.mDesc;
            unsigned streamByteSize = nodeData->GetStreamStride(streamIndex) * vertexCount;

            //has the size changed? or are we trying to update the contents of a non dynamic resource?
            if (bufferData.mBuffer != nullptr && (streamByteSize > streamDesc.ByteWidth || (streamDesc.Usage == D3D11_USAGE_DEFAULT && !isCompute)))
            {
                //erase this buffer
                bufferData.mBuffer = nullptr;
                bufferData.mUav = nullptr;
                bufferData.mSrv = nullptr;
            }

            if (bufferData.mBuffer == nullptr)
            {
                Pegasus::Render::DXCreateBuffer(
                    device,
                    streamByteSize,
                    meshGpuData->mVertexCount,
                    configuration.GetIsDynamic(),
                    isCompute ? nullptr : nodeData->GetStream<void>(streamIndex),
                    (D3D11_BIND_FLAG)(D3D11_BIND_VERTEX_BUFFER | (isCompute ? (D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE) : 0)),
                    bufferData
                );
            }
            else if (!isCompute)
            {
                PG_ASSERT(streamDesc.Usage == D3D11_USAGE_DYNAMIC);
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                if (context->Map(bufferData.mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
                {
                    PG_ASSERTSTR(mappedResource.pData != nullptr,"map returned a null pointer of data!");
                    Pegasus::Utils::Memcpy(mappedResource.pData, nodeData->GetStream<void>(streamIndex), streamByteSize);
                    context->Unmap(bufferData.mBuffer, 0);
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
        Pegasus::Render::DXBufferGPUData& bufferData = meshGpuData->mIndexStream;
        D3D11_BUFFER_DESC& streamDesc = bufferData.mDesc;
        unsigned streamByteSize = nodeData->GetIndexCount() * sizeof(unsigned short);

        if (bufferData.mBuffer != nullptr && (streamByteSize > streamDesc.ByteWidth || (streamDesc.Usage == D3D11_USAGE_DEFAULT && !isCompute)))
        {
            bufferData.mBuffer = nullptr;
        }

        meshGpuData->mIndexCount = nodeData->GetIndexCount();
        PG_ASSERTSTR( nodeData->GetIndexCount() != 0, "Cannot pass 0 size index buffer. Forgot to call AllocIndices on meshData?");
        if (bufferData.mBuffer == nullptr)
        {
            Pegasus::Render::DXCreateBuffer(
                device,
                streamByteSize,
                meshGpuData->mIndexCount,
                configuration.GetIsDynamic(),
                isCompute ? nullptr : nodeData->GetIndexBuffer(),
                (D3D11_BIND_FLAG)(D3D11_BIND_INDEX_BUFFER | (isCompute ? (D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE) : 0)),
                bufferData
            );
        }
        else if (!isCompute)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            if (context->Map(bufferData.mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
            {
                PG_ASSERTSTR(mappedResource.pData != nullptr, "map returned a null pointer of data!");
                Pegasus::Utils::Memcpy(mappedResource.pData, nodeData->GetIndexBuffer(), streamByteSize);
                context->Unmap(bufferData.mBuffer, 0);
            }
            else
            {
                PG_FAILSTR("GPU Map failed!");
            }
        }
    }

    if (configuration.GetIsDrawIndirect())
    {
        Pegasus::Render::DXBufferGPUData& bufferData = meshGpuData->mIndirectDrawStream;
        Pegasus::Render::DXCreateBuffer(
            device,
            5 * 4, //5 arguments, 4 bytes each.
            5,
            false, //not dynamic
            nullptr, //no init data
            (D3D11_BIND_FLAG)(D3D11_BIND_UNORDERED_ACCESS),
            bufferData,
            D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS
        );
    }

    //reset all the existing input layouts
    for (int i = 0; i < meshGpuData->mInputLayoutTableCount; ++i)
    {
        meshGpuData->mInputLayoutTable[i].mInputLayout = nullptr; //delete
    }
    meshGpuData->mInputLayoutTableCount = 0;

    //translate mesh configuration into input layout structure
    int attributeCount = meshInputLayout.GetAttributeCount();
    PG_ASSERTSTR(attributeCount < Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC, "Exceeding the maximum number of input layout elements. Truncating the rest");
    //sanity check for attribute count
    attributeCount = attributeCount < Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC ? attributeCount : Pegasus::Render::DXMeshGPUData::MAX_INPUT_ELEMENTS_DESC - 1;
    
    //clear the previous translated description
    meshGpuData->mInputElementsCount = 0;
    for (int i = 0; i < attributeCount; ++i)
    {
        const Pegasus::Mesh::MeshInputLayout::AttrDesc& attr = meshInputLayout.GetAttributeDesc(i);
        PG_ASSERT(
            attr.mType < Pegasus::Core::FORMAT_MAX_COUNT &&
            attr.mSemanticIndex < Pegasus::Mesh::MeshInputLayout::SEMANTIC_COUNT
        );
        
        DXGI_FORMAT fmt = GetDxFormat(attr.mType);
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
            meshGpuData->mVertexStreams[i].mBuffer = nullptr;
            meshGpuData->mVertexStreams[i].mUav = nullptr;
            meshGpuData->mVertexStreams[i].mSrv = nullptr;
            meshGpuData->mVertexBuffers[i]->SetInternalData(nullptr);
        }
        meshGpuData->mIndexStream.mBuffer = nullptr;
        meshGpuData->mIndexStream.mUav = nullptr;
        meshGpuData->mIndexStream.mSrv = nullptr;
        meshGpuData->mIndexBuffer->SetInternalData(nullptr);

        meshGpuData->mIndirectDrawStream.mBuffer = nullptr;
        meshGpuData->mIndirectDrawStream.mUav = nullptr;
        meshGpuData->mIndirectDrawStream.mSrv = nullptr;
        meshGpuData->mDrawIndirectBuffer->SetInternalData(nullptr);


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
