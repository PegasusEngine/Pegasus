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

class DXMeshFactory : public Pegasus::Mesh::IMeshFactory
{
public:
    DXMeshFactory() : mAllocator(nullptr){}
    virtual ~DXMeshFactory(){}
    
    
    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);
    
    virtual void GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData);
    
    virtual void DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData);
    
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

    int vertexCount = nodeData->GetVertexCount();

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
