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
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace0)
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace1)
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace2)
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace3)
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace4)
    IMPLEMENT_PROPERTY(BoxGenerator, EnableFace5)
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
        INIT_PROPERTY(EnableFace0)
        INIT_PROPERTY(EnableFace1)
        INIT_PROPERTY(EnableFace2)
        INIT_PROPERTY(EnableFace3)
        INIT_PROPERTY(EnableFace4)
        INIT_PROPERTY(EnableFace5)
    END_INIT_PROPERTIES()
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

    unsigned int faceEnableMask =
                     (GetEnableFace0() ? 1 : 0) 
                   |((GetEnableFace1() ? 1 : 0) << 1)
                   |((GetEnableFace2() ? 1 : 0) << 2)
                   |((GetEnableFace3() ? 1 : 0) << 3)
                   |((GetEnableFace4() ? 1 : 0) << 4)
                   |((GetEnableFace5() ? 1 : 0) << 5)
    ;
    
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

    StdVertex * stream = meshData->GetStream<StdVertex>(0);

    static const int sFaceSwizzles[][3] =
    { /*x  y  z */
        { 0, 1, 2 },
        { 0, 1, 2 },
        { 2, 0, 1 },
        { 2, 0, 1 },
        { 1, 2, 0 },
        { 1, 2, 0 }
    };

    //generate front face (XY,Z=1)
    int vertCountInt = subdivisionCount + 1;
    float vertCountFloat = static_cast<float>(vertCountInt);
    float len = static_cast<float>(subdivisionCount);
    float halfLen = 0.5f*len;
    float invHalfLen = 1.0f / halfLen;
    int currIdx = 0;
    int faceOffsets[6];
    for (int face = 0; face < 6; ++face)
    {
        if (!((faceEnableMask >> face) & 1))
        {
            continue;
        }
        faceOffsets[face] = currIdx;
        int i = 0;
        float sign = (face % 2) == 0 ? 1.0f : -1.0f;
        for (float r = 0.0f; r < vertCountFloat; r += 1.0f, ++i)
        {
            int j = 0;
            for (float c = 0.0f; c < vertCountFloat; c += 1.0f, ++j)
            {
                float x = invHalfLen*(c - halfLen);
                float y = -invHalfLen*(r - halfLen);
                Vec4 pos(x,y,1.0f,1.0f);
                Vec3 norm(0.0f,0.0f,1.0f);
                stream[currIdx].position = Vec4(sign*Vec3(pos.v[sFaceSwizzles[face][0]], pos.v[sFaceSwizzles[face][1]], pos.v[sFaceSwizzles[face][2]]) * GetCubeExtends(),1.0f);
                stream[currIdx].normal = sign*Vec3(norm.v[sFaceSwizzles[face][0]], norm.v[sFaceSwizzles[face][1]], norm.v[sFaceSwizzles[face][2]]);
                stream[currIdx].uv = Vec2(x,y)*0.5f + 0.5f;
                currIdx++;
            }
        }
    }

	//set the index data
    const short indexesPerFace = 6;
    meshData->AllocateIndexes(indexesPerFace * subdivisionCount * subdivisionCount * 6/*faces*/);
    unsigned short * idx = meshData->GetIndexBuffer();
    int indexOffset = 0;
    for (unsigned short face = 0; face < 6; ++face)
    {
        if (!((faceEnableMask >> face) & 1))
        {
            continue;
        }
        for (int j = 0; j < subdivisionCount; ++j)
        {
            for (int i = 0; i < subdivisionCount; ++i)
            {
                int offset = faceOffsets[face] + i * vertCountInt + j;
                const unsigned short offsetS = (unsigned short)offset;
                const unsigned short a = offsetS;
                const unsigned short b = a + 1;
                const unsigned short c = a + (unsigned short)vertCountInt;
                const unsigned short d = c + 1;
                if ((face % 2) == 0)
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
