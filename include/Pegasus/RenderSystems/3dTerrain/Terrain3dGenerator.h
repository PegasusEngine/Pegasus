/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Terrain3dGenerator.h
//! \author Kleber Garcia
//! \date   July 2nd, 2016
//! \brief  Terrain mesh generator.

#ifndef PEGASUS_TERRAIN_3D_MESH_GENERATOR
#define PEGASUS_TERRAIN_3D_MESH_GENERATOR

#include "Pegasus/RenderSystems/Config.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace RenderSystems
{

//! Mesh cube generator. Generates a cube
class Terrain3dGenerator : public Mesh::MeshGenerator
{
    DECLARE_MESH_GENERATOR_NODE(Terrain3dGenerator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(Terrain3dGenerator, MeshGenerator)
        DECLARE_PROPERTY(float, TerrainSeed, 0.0f)
    END_DECLARE_PROPERTIES()

public:

    //! Cube generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    Terrain3dGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~Terrain3dGenerator(){}

    //! Compute mode.
    virtual Graph::Node::Mode GetMode() const { return Graph::Node::COMPUTE; }

    //! Override of Update
    virtual bool Update();

    //! creates internally render resources if they have not been created.
    void CreateComputeTerrainResources();

    //! Sets the offset and scale of this block. for the GPU to generate the mesh.
    void SetOffsetScale(float scale, const Math::Vec3 offset);

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

private:
    bool mIsComputeResourcesAllocated;

    Render::BufferRef mCaseUniformBuffer;

    Render::BufferRef mIndexBuffer;
    Render::BufferRef mVertexBuffer;
    Render::BufferRef mNormalBuffer;
    Render::BufferRef mDrawIndirectBuffer;

    RenderSystems::Terrain3dSystem::TerrainResources mResources;
    int mProgramVersions[Terrain3dSystem::PROGRAM_COUNT];
};


}
}

#endif
#endif
