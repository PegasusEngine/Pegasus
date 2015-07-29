/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	QuadGenerator.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh generator of a quad 

#include "Pegasus/Mesh/Generator/QuadGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace Mesh
{

//! Property implementations 
BEGIN_IMPLEMENT_PROPERTIES2(QuadGenerator)
    IMPLEMENT_PROPERTY2(QuadGenerator, QuadExtends)
END_IMPLEMENT_PROPERTIES2(QuadGenerator)

QuadGenerator::QuadGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator) 
:   MeshGenerator(nodeAllocator, nodeDataAllocator)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(QuadGenerator)
        INIT_PROPERTY2(QuadExtends)
    END_INIT_PROPERTIES()

    //setting up the basic format of node data

    //set not indexed
    mConfiguration.SetIsIndexed(false);

    //quad rendering
    mConfiguration.SetMeshPrimitiveType(MeshConfiguration::TRIANGLE);

    //this mesh only contains position and uvs
    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV);
}

//----------------------------------------------------------------------------

QuadGenerator::~QuadGenerator()
{
}

//----------------------------------------------------------------------------

void QuadGenerator::GenerateData()
{
    using namespace Pegasus::Math;

    GRAPH_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr);

    meshData->AllocateVertexes(6);

    struct Vertex
    {
        Vec4 position; 
        Vec2 uv; 
    };

    Vertex * stream = meshData->GetStream<Vertex>(0);
    PG_ASSERT(stream);
    
    //two triangles, 2 coordinates each, 12 coords
    float quadcoords[12] = { -1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1};

    //copying them to the position array
    for (int i = 0; i < 6; ++i)
    {
        stream[i].position.x = quadcoords[i*2];
        stream[i].position.y = quadcoords[i*2 + 1];
        stream[i].position.z = stream[0].position.w = 0;
        stream[i].uv.s = stream[i].position.x * 0.5f + 0.5f;
        stream[i].uv.t = stream[i].position.y * 0.5f + 0.5f;
    }

    GRAPH_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

} //Mesh
} //Pegasus
