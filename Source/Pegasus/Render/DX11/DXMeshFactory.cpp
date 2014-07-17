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

class DXMeshFactory : public Pegasus::Mesh::IMeshFactory
{
public:
    DXMeshFactory(){}
    virtual ~DXMeshFactory(){}
    
    
    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);
    
    virtual void GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData);
    
    virtual void DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData);
};

void DXMeshFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
}

void DXMeshFactory::GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData)
{
}

void DXMeshFactory::DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData)
{
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
