/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MarchingCubeMeshGenerator.h
//! \author Kleber Garcia
//! \date   June 28th, 2016
//! \brief  Single cube that represents a marching cube, contains the triangles by such marching cube.

#ifndef PEGASUS_MARCHING_CUBE_MESH_GENERATOR
#define PEGASUS_MARCHING_CUBE_MESH_GENERATOR

#include "Pegasus/Mesh/MeshGenerator.h"

#include "Pegasus/RenderSystems/3dTerrain/Terrain3dGenerator.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

namespace Pegasus
{
namespace RenderSystems
{

//! Mesh cube generator. Generates a cube
class MarchingCubeMeshGenerator : public Mesh::MeshGenerator
{
    DECLARE_MESH_GENERATOR_NODE(MarchingCubeMeshGenerator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(MarchingCubeMeshGenerator, MeshGenerator)
        DECLARE_PROPERTY(int, CaseNumber, 0)
    END_DECLARE_PROPERTIES()

public:

    //! Cube generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    MarchingCubeMeshGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~MarchingCubeMeshGenerator();

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};


}
}

#endif
#endif
