/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CylinderGenerator.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh generator of a quad 

#include "Pegasus/Mesh/Generator/CylinderGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Mesh {


//! Property implementations 
BEGIN_IMPLEMENT_PROPERTIES(CylinderGenerator)
    IMPLEMENT_PROPERTY(CylinderGenerator, CylinderCapRadius)
    IMPLEMENT_PROPERTY(CylinderGenerator, CylinderHeight)
    IMPLEMENT_PROPERTY(CylinderGenerator, CylinderRingCuts)
    IMPLEMENT_PROPERTY(CylinderGenerator, CylinderFaceCount)
END_IMPLEMENT_PROPERTIES(CylinderGenerator)

//----------------------------------------------------------------------------------------

CylinderGenerator::CylinderGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator) 
:   MeshGenerator(nodeAllocator, nodeDataAllocator)
{
    BEGIN_INIT_PROPERTIES(CylinderGenerator)
        INIT_PROPERTY(CylinderCapRadius)
        INIT_PROPERTY(CylinderHeight)
        INIT_PROPERTY(CylinderRingCuts)
        INIT_PROPERTY(CylinderFaceCount)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------

CylinderGenerator::~CylinderGenerator()
{
}

//----------------------------------------------------------------------------

void CylinderGenerator::GenerateData()
{
    using namespace Pegasus::Math;

    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);

    MeshDataRef meshData = GetData();
    PG_ASSERT(meshData != nullptr);

    //clamp and protect min and max size
    int faceCount = GetCylinderFaceCount();
    if (faceCount < 3)
    {
        PG_LOG('ERR_', "Cylinder face count must be greater than 3.");
        faceCount = 3;
    }

    int ringCuts = GetCylinderRingCuts();
    if (ringCuts < 0)
    {
        PG_LOG('ERR_', "Cylinder ring cuts must be greater than 0.");
        ringCuts = 0;
    }

    ringCuts += 2; //we consider the first two caps as ring cuts for simplicity sake

    //figure out sizes
    const int capVertexCount = faceCount + 1;
    const int capIndexCount = 3*faceCount; //1 triangle per face
    const int ringVertexCounts = ringCuts*faceCount; //2 extra because we consider the top and bottom cap
    const int tubeIndexCounts = faceCount * (ringCuts - 1) * 3 * 2; 
    const float halfHeight = GetCylinderHeight() * 0.5f;

    //layout of mesh is going to be Cap + Ring + ... + Ring + Cap
    meshData->AllocateVertexes(capVertexCount * 2 + ringVertexCounts);
    meshData->AllocateIndexes(capIndexCount * 2 + tubeIndexCounts);

    StdVertex * stream = meshData->GetStream<StdVertex>(0);
    unsigned short* indexBuffer = meshData->GetIndexBuffer();
    int nextIndex = 0;
    PG_ASSERT(stream);


    //Top cap:
    Math::Vec2 pageScale(0.5f,0.5f);
    Math::Vec2 pageOffset(0.0f,0.0f);
    CreateRing(
       stream,0, halfHeight, faceCount,
       indexBuffer, nextIndex, pageOffset, pageScale, /*isCap*/true, /*isLowerCap*/false);

    //Bottom cap:
    pageOffset = Math::Vec2(0.0f,0.5f);
    CreateRing(
       stream,capVertexCount, -halfHeight, faceCount,
       indexBuffer, nextIndex, pageOffset, pageScale, /*isCap*/true, /*isLowerCap*/true);

    float heightPerRing = GetCylinderHeight() / static_cast<float>(ringCuts - 1.0f);
    int currVertexOffset = capVertexCount * 2;

    float ringUvSpacing = 0.5f / (float)(ringCuts - 1);
    pageScale = Math::Vec2(0.5f, 1.0f);
    //create rings in between.
    for (int r = 0; r < ringCuts ; ++r)
    {
        pageOffset = Math::Vec2(0.5f + (float)r*ringUvSpacing, 0.0f);
        //Support cap:
        CreateRing(
           stream, currVertexOffset, halfHeight - ((float)r) * heightPerRing, faceCount,
           indexBuffer, nextIndex, pageOffset, pageScale, /*isCap*/false, /*isLowerCap*/false);

        //bind rings
        if (r != (ringCuts - 1))
        {
            for (int f = 0; f < faceCount; ++f)
            {
                int a = currVertexOffset + f;
                int b = currVertexOffset + ((f + 1) % faceCount);
                int c = a + faceCount;
                int d = b + faceCount;
                indexBuffer[nextIndex++] = a;
                indexBuffer[nextIndex++] = c;
                indexBuffer[nextIndex++] = d;
                indexBuffer[nextIndex++] = d;
                indexBuffer[nextIndex++] = b;
                indexBuffer[nextIndex++] = a;
            }
        }

        currVertexOffset += faceCount;
    }
    
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

void CylinderGenerator::CreateRing(
   StdVertex* destination,
   int destinationOffset,
   float zVal,
   int faceCount,
   unsigned short* indexBuffer,
   int& nextIndex,
   const Math::Vec2& uvOffset,
   const Math::Vec2& uvScale,
   bool isCap,
   bool isLowerCap)
{
    float angleDelta = Math::P_2_PI / static_cast<float>(faceCount);
    Math::Vec3 capNormal(0.0f,0.0f,zVal > 0.0f ? 1.0f : -1.0f);
    float vertUvSpacing = 1.0f/((float)faceCount);
    for (int f = 0; f < faceCount; ++f)
    {
        float currAngle = static_cast<float>(f)*angleDelta;
        Math::Vec2 cosSin(Math::Cos(currAngle), Math::Sin(currAngle));
        Math::Vec3 vertPos(cosSin * GetCylinderCapRadius(), zVal);
        StdVertex* currVert = destination + destinationOffset + f;
        currVert->position = Math::Vec4(vertPos, 1.0f);
        currVert->normal = isCap ? capNormal : Math::Vec3(cosSin, 0.0f);
        if (isCap)
        {
            currVert->uv = cosSin * 0.5f + 0.5f;
        }
        else
        {
            currVert->uv = Math::Vec2(0.0f, (float)f * vertUvSpacing);
        }

        currVert->uv = (uvScale*currVert->uv) + uvOffset;
    }

    if (isCap)
    {
        //create center point at the end
        {
            destination[destinationOffset + faceCount].position = Math::Vec4(0.0f,0.0f,zVal,1.0f);
            destination[destinationOffset + faceCount].normal = capNormal;
            destination[destinationOffset + faceCount].uv = uvScale*Math::Vec2(0.5f,0.5f) + uvOffset;
        }

        int a = destinationOffset + faceCount; //this will be the center
        for (int f = 0; f < faceCount; ++f)
        {
            int b = destinationOffset + f;
            int c = destinationOffset + (f + 1) % faceCount;
            indexBuffer[nextIndex++] = a; 
            indexBuffer[nextIndex++] = isLowerCap ? c : b;
            indexBuffer[nextIndex++] = isLowerCap ? b : c;
        }
    }


}

} //Mesh
} //Pegasus
