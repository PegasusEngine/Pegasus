#include "Pegasus/Mesh/Generator/QuadGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace Mesh
{

QuadGenerator::QuadGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator) 
:   MeshGenerator(nodeAllocator, nodeDataAllocator)
{
    //setting up the basic format of node data

    //allocate 4 vertexes
    mConfiguration.SetVertexCount(6); 

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

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr);

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
}

} //Mesh
} //Pegasus
