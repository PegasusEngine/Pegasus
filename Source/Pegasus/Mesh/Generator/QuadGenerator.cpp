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
    mConfiguration.SetVertexCount(4); 

    //set not indexed
    mConfiguration.SetIsIndexed(false);

    //quad rendering
    mConfiguration.SetMeshPrimitiveType(MeshConfiguration::QUAD);

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

    //build quad procedurally
    for (int i = 0; i < 4; ++i)
    {
        stream[i].position.x = i % 2 ? -1.0f : 1.0f;
        stream[i].position.y = i / 2 ? 1.0f : -1.0f;
        stream[i].position.z = stream[0].position.w = 0;
        stream[i].uv.s = stream[i].position.x * 0.5f + 0.5f;
        stream[i].uv.t = stream[i].position.y * 0.5f + 0.5f;
    }
}

} //Mesh
} //Pegasus
