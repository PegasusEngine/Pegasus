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

#include "../Source/Pegasus/Render/GL/GLGPUDataDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Render/MeshFactory.h"
#include "Pegasus/Mesh/MeshData.h"

#define VAO_TABLE_INCREMENT 20

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
    Pegasus::Render::OGLMeshGPUData * GetOrAllocateGPUData(Pegasus::Mesh::MeshData * nodeData);
    Pegasus::Alloc::IAllocator * mAllocator;
};


Pegasus::Render::OGLMeshGPUData * GLMeshFactory::GetOrAllocateGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    Pegasus::Render::OGLMeshGPUData * meshGPUData = nullptr;
    if (nodeData->GetNodeGPUData() == nullptr)
    {
        meshGPUData = PG_NEW(
            mAllocator,
            -1,
            "Mesh GPU Data",
            Pegasus::Alloc::PG_MEM_TEMP)
            Pegasus::Render::OGLMeshGPUData();

        //setup the draw state
        meshGPUData->mDrawState.mIsIndexed = false;
        meshGPUData->mDrawState.mIndexCount  = 0;
        meshGPUData->mDrawState.mVertexCount = 0;
        meshGPUData->mDrawState.mPrimitive = GL_TRIANGLES; // defaulting to triangles

        // setting up empty vao table
        meshGPUData->mVAOTableSize = VAO_TABLE_INCREMENT;
        meshGPUData->mVAOTableCount = 0;
        meshGPUData->mVAOTable  = PG_NEW_ARRAY (
            mAllocator,
            -1,
            "Mesh GPU VAO table",
            Pegasus::Alloc::PG_MEM_TEMP,
            Pegasus::Render::OGLMeshGPUData::VAOEntry,
            VAO_TABLE_INCREMENT
        );

        GLuint VAONames[VAO_TABLE_INCREMENT];
        glGenVertexArrays(VAO_TABLE_INCREMENT, VAONames);

        for (int i = 0; i < VAO_TABLE_INCREMENT; ++i)
        {
            meshGPUData->mVAOTable[i].mVAOName = VAONames[i];
        }

        // setting up stream table
        for (int i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            meshGPUData->mBufferTable[i] = GL_INVALID_INDEX;
        }

        meshGPUData->mIndexBuffer = GL_INVALID_INDEX;

        //set the brand new mesh gpu data
        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(meshGPUData));
    }
    else
    {
        meshGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLMeshGPUData, nodeData->GetNodeGPUData());
    }
    return meshGPUData;
}

//! API function that converts from a nodeData to a GPU valid OpenGL handle for a mesh
void GLMeshFactory::GenerateMeshGPUData(Pegasus::Mesh::MeshData * nodeData)
{

    // must match the primitive enums declared in MeshConfiguration.cpp
    static GLuint sPrimitiveMap[Pegasus::Mesh::MeshConfiguration::PRIMITIVE_COUNT] = {
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
        GL_LINE,
        GL_LINE_STRIP,
        GL_POINT
    };

    Pegasus::Render::OGLMeshGPUData * gpuData = GetOrAllocateGPUData(nodeData);
    // count the streams we have and allocate appropiate size of streams.
    const Pegasus::Mesh::MeshConfiguration& meshConfig = nodeData->GetConfiguration();

    // configuring draw state
    gpuData->mDrawState.mVertexCount = nodeData->GetVertexCount();
    gpuData->mDrawState.mPrimitive = sPrimitiveMap[meshConfig.GetMeshPrimitiveType()];

    for (int stream = 0; stream < MESH_MAX_STREAMS; ++stream)
    {
        int byteSize = nodeData->GetStreamByteSize(stream);
        if (byteSize > 0)
        {
            if (gpuData->mBufferTable[stream] == GL_INVALID_INDEX)
            {
                glGenBuffers(1, &gpuData->mBufferTable[stream]);
            }
            glBindBuffer(GL_COPY_WRITE_BUFFER, gpuData->mBufferTable[stream]);
            glBufferData(GL_COPY_WRITE_BUFFER, byteSize, nodeData->GetStream<void>(stream),
                meshConfig.GetIsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
    }

    if (meshConfig.GetIsIndexed())
    {
        gpuData->mDrawState.mIsIndexed = true;
        gpuData->mDrawState.mIndexCount = nodeData->GetIndexCount();
        if (gpuData->mIndexBuffer == GL_INVALID_INDEX)
        {
            glGenBuffers(1, &gpuData->mIndexBuffer);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuData->mIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * nodeData->GetIndexCount(), 
                     nodeData->GetIndexBuffer(),
                     meshConfig.GetIsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    }
    else
    {
        gpuData->mDrawState.mIsIndexed = false;
    }
}

//! API function that deletes any GPU data inside the node data, if any
void GLMeshFactory::DestroyNodeGPUData(Pegasus::Mesh::MeshData * nodeData)
{
    if (nodeData->GetNodeGPUData() != nullptr)
    {
        Pegasus::Render::OGLMeshGPUData * meshGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::OGLMeshGPUData, nodeData->GetNodeGPUData());

        // deleting VAO table
        for (int i = 0; i < VAO_TABLE_INCREMENT; ++i)
        {
            if (meshGPUData->mVAOTable[i].mVAOName != GL_INVALID_INDEX)
            {
                glDeleteVertexArrays(1, &meshGPUData->mVAOTable[i].mVAOName);
            }
        }
        PG_DELETE_ARRAY(mAllocator, meshGPUData->mVAOTable);

        // destroying stream table
        for (int i = 0; i < MESH_MAX_STREAMS; ++i)
        {
            if (meshGPUData->mBufferTable[i] != GL_INVALID_INDEX)
            {
                glDeleteBuffers(1, &meshGPUData->mBufferTable[i]);
            }
        }

        if (meshGPUData->mIndexBuffer != GL_INVALID_INDEX)
        {
            glDeleteBuffers(1, &meshGPUData->mIndexBuffer);
        }

        // delete final reference to GPU data
        PG_DELETE(mAllocator, meshGPUData);

        // remove the gpu data from the node and call it a day
        nodeData->SetNodeGPUData(nullptr);
    }
}

//! define a global static mesh factory API
static GLMeshFactory gGlobalMeshFactory;

Pegasus::Mesh::IMeshFactory * Pegasus::Render::GetRenderMeshFactory()
{
    return &gGlobalMeshFactory;
}

#endif //PEGASUS_GAPI_GL
