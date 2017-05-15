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

namespace Pegasus {
namespace Mesh {

//! Property implementations
BEGIN_IMPLEMENT_PROPERTIES(BoxGenerator)
    IMPLEMENT_PROPERTY(BoxGenerator, CubeExtends)
    IMPLEMENT_PROPERTY(BoxGenerator, FaceSubdivision)
END_IMPLEMENT_PROPERTIES(BoxGenerator)

//----------------------------------------------------------------------------------------

BoxGenerator::BoxGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
              Pegasus::Alloc::IAllocator* nodeDataAllocator) 
: MeshGenerator(nodeAllocator, nodeDataAllocator), mVertsPerFace(0)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(BoxGenerator)
        INIT_PROPERTY(CubeExtends)
        INIT_PROPERTY(FaceSubdivision)
    END_INIT_PROPERTIES()

    //this mesh only contains position UVs and normals
    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV | MeshInputLayout::USE_NORMAL);
}

//----------------------------------------------------------------------------------------

BoxGenerator::~BoxGenerator()
{
}

//----------------------------------------------------------------------------------------

void BoxGenerator::GenerateData()
{
    using namespace Pegasus::Math;
    
   PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);
    
    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr); 
    mVertsPerFace = 4;
    int subdivisionCount = 1;
    if (GetFaceSubdivision() >= 1)
    {
        mVertsPerFace = GetFaceSubdivision() + 1;
        mVertsPerFace *= mVertsPerFace;
        subdivisionCount = GetFaceSubdivision();
    }
    meshData->AllocateVertexes(6 /*sides*/ * (mVertsPerFace));

    struct Vertex
    {
        Vec4 position; 
        Vec3 normal;        
        Vec2 uv; 
    };

    Vertex * stream = meshData->GetStream<Vertex>(0);

    //generate front face (XY,Z=1)
    int vertCountInt = subdivisionCount + 1;
    float vertCountFloat = static_cast<float>(vertCountInt);
    float len = static_cast<float>(subdivisionCount);
    float halfLen = 0.5f*len;
    float invHalfLen = 1.0f / halfLen;
    {
        int i = 0;
        for (float r = 0.0f; r < vertCountFloat; r += 1.0f, ++i)
        {
            int j = 0;
            for (float c = 0.0f; c < vertCountFloat; c += 1.0f, ++j)
            {
                float x = invHalfLen*(c - halfLen);
                float y = -invHalfLen*(r - halfLen);
                int idx = i * vertCountInt + j;
                stream[idx].position = Vec4(x,y,1.0f,1.0f);
                stream[idx].normal = Vec3(0.0f,0.0f,1.0f);
                stream[idx].uv = Vec2(x,y)*0.5f + 0.5f;
            }
        }
    }

    static const int sFaceSwizzles[][3] =
    { /*x  y  z */
        { 0, 1, 2 },
        { 0, 1, 2 },
        { 2, 0, 1 },
        { 2, 0, 1 },
        { 1, 2, 0 },
        { 1, 2, 0 }
    };
    //copy and rotate front face on other faces
    for (int face = 5; face >= 0; --face)
    {
        int offset = face * mVertsPerFace;
        float sign = (face % 2) == 0 ? 1.0f : -1.0f;
        for (int v = 0; v < mVertsPerFace; ++v)
        {
            Vertex* targetVert = stream + offset + v;
            const Vec4& posCpy = stream[v].position;
            const Vec3& norCpy = stream[v].normal;
            const Vec2& uvCpy = stream[v].uv;
            targetVert->position = Vec4(sign*Vec3(posCpy.v[sFaceSwizzles[face][0]], posCpy.v[sFaceSwizzles[face][1]], posCpy.v[sFaceSwizzles[face][2]]) * GetCubeExtends(),1.0f);
            targetVert->normal = sign*Vec3(norCpy.v[sFaceSwizzles[face][0]], norCpy.v[sFaceSwizzles[face][1]], norCpy.v[sFaceSwizzles[face][2]]);
            targetVert->uv = uvCpy;
        }
    }


	//set the index data
    const short indexesPerFace = 6;
    meshData->AllocateIndexes(indexesPerFace * subdivisionCount * subdivisionCount * 6/*faces*/);
    unsigned short * idx = meshData->GetIndexBuffer();
    int indexOffset = 0;
    for (unsigned short f = 0; f < 6; ++f)
    {
        for (int j = 0; j < subdivisionCount; ++j)
        {
            for (int i = 0; i < subdivisionCount; ++i)
            {
                int offset = f*mVertsPerFace + i * vertCountInt + j;                
                const unsigned short offsetS = (unsigned short)offset;
                const unsigned short a = offsetS;
                const unsigned short b = a + 1;
                const unsigned short c = a + (unsigned short)vertCountInt;
                const unsigned short d = c + 1;
                if ((f % 2) == 0)
                {
                    idx[indexOffset++] = a;
                    idx[indexOffset++] = c;
                    idx[indexOffset++] = d;
                    idx[indexOffset++] = d;
                    idx[indexOffset++] = b;
                    idx[indexOffset++] = a;
                }
                else
                {
                    idx[indexOffset++] = a;
                    idx[indexOffset++] = b;
                    idx[indexOffset++] = d;
                    idx[indexOffset++] = d;
                    idx[indexOffset++] = c;
                    idx[indexOffset++] = a;
                }
            }
        }
    }

    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}


}
}
