/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BoxGenerator.cpp
//! \author	Kleber Garcia
//! \date	June 16th 2014
//! \brief	Cube Generator

#include "Pegasus/Mesh/Generator/BoxGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace Mesh
{


BoxGenerator::BoxGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
              Pegasus::Alloc::IAllocator* nodeDataAllocator) 
: MeshGenerator(nodeAllocator, nodeDataAllocator)
{

    mConfiguration.SetIsIndexed(true);

    mConfiguration.SetMeshPrimitiveType(MeshConfiguration::TRIANGLE);

    //this mesh only contains position uvs and normals
    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV | MeshInputLayout::USE_NORMAL);
}

BoxGenerator::~BoxGenerator()
{
}

void BoxGenerator::GenerateData()
{
    using namespace Pegasus::Math;
    
    GRAPH_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);
    
    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 
    meshData->AllocateVertexes(24);

    struct Vertex
    {
        Vec4 position; 
        Vec3 normal;        
        Vec2 uv; 
    };

    Vertex * stream = meshData->GetStream<Vertex>(0);

    //use bits to count the permutation of axis coordinates that change.
	// generate a cube based on this
    for (int i = 0; i < 24; ++i)
    {
		// 2 coordinates change, and one remains constant (the face)
        float v1 = (float) ((i & 1) << 1) - 1;
        float v2 = (float)  (i & 2) - 1;
        float face = (float)  (((i & 4) - 2) >> 1);

		//precompute conditional gates
        int g8 = (i >= 8) ? 1 : 0;
        int ng8 = g8 ^ 1;
        int b8a16 = (i >= 8 && i < 16) ? 1 : 0;
        int nb8a16 = b8a16 ^ 1;
        int g16 = (i >= 16) ? 1 : 0;
        int ng16 = g16 ^ 1;

		//combine
        float z =  v2*g8 + face*ng8;
        float y =  v2 * ng8 + v1 * g16 - (face * b8a16); //use a negative face here to invert the logic.
        float x = face * g16 + v1 * ng16;
        stream[i].position = Vec4(x,y,z,1); 
        stream[i].normal = Vec3 (face * g16, -face * b8a16, face * ng8);
		
		//set the uvs by broadening the area that each face can cover the most
        stream[i].uv = Vec2(v1 + (i / 8), v2 + (-face + 1.0f)*0.5f) / 3.0f;
    }

	//set the index data
    meshData->AllocateIndexes(36);
    unsigned short * idx = meshData->GetIndexBuffer();
    for (unsigned short f = 0; f < 6; ++f)
    {
        unsigned short a = f * 4;
        unsigned short b = a + 1;
        unsigned short c = b + 1;
        unsigned short d = c + 1;

		//flip depending on the face (if its positive or negative)
		//faces are stored as negative, positive etc
        idx[f*6]     = a;
        idx[f*6 + 1] = f % 2 ? b : c;
        idx[f*6 + 2] = f % 2 ? c : b;
        
        idx[f*6 + 3] = b;
        idx[f*6 + 4] = f % 2 ? d : c;
        idx[f*6 + 5] = f % 2 ? c : d;

    }

    GRAPH_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}


}
}
