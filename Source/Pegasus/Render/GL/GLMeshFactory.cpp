/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLMeshFactory.cpp
//! \author Kleber Garcia
//! \date   16th May 2014
//! \brief  Mesh factory implementation. Provided to the mesh package

#if PEGASUS_GAPI_GL

#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Render/MeshFactory.h"

//! internal definition of mesh factory API
class GLMeshFactory : public Pegasus::Mesh::IMeshFactory
{
public:
    GLMeshFactory() : mAllocator(nullptr){}

    virtual ~GLMeshFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator)
    {
        mAllocator = allocator;
    }

    virtual void GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData);
    virtual void DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData);
private:
    Pegasus::Alloc::IAllocator * mAllocator;
};


//! API function that converts from a nodeData to a gpu valid opengl handle for a mesh
void GLMeshFactory::GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    //!TODO - implement
}

//! API function that deletes any GPU data inside the node data, if any
void GLMeshFactory::DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    //!TODO - implement
}

//! define a global static mesh factory api
static GLMeshFactory gGlobalMeshFactory;

Pegasus::Mesh::IMeshFactory * Pegasus::Render::GetRenderMeshFactory()
{
    return &gGlobalMeshFactory;
}

#endif //PEGASUS_GAPI_GL
