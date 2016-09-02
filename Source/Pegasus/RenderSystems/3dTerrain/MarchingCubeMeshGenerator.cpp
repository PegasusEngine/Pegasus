/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MarchingCubeMeshGenerator.cpp
//! \author Kleber Garcia
//! \date   July 2nd, 2016
//! \brief  Marching cube geneator of meshes.

#include "Pegasus/RenderSystems/3dTerrain/MarchingCubeMeshGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Math/Vector.h"

using namespace Pegasus;
using namespace Pegasus::Mesh;
using namespace Pegasus::RenderSystems;
using namespace Pegasus::Math;

extern RenderSystems::Terrain3dSystem* g3dTerrainSystemInstance;


BEGIN_IMPLEMENT_PROPERTIES(MarchingCubeMeshGenerator)
    IMPLEMENT_PROPERTY(MarchingCubeMeshGenerator, CaseNumber)
END_IMPLEMENT_PROPERTIES(MarchingCubeMeshGenerator)

MarchingCubeMeshGenerator::MarchingCubeMeshGenerator(Pegasus::Alloc::IAllocator* nodeAllocator,
                          Pegasus::Alloc::IAllocator* nodeDataAllocator) : MeshGenerator(nodeAllocator, nodeDataAllocator)
{
    BEGIN_INIT_PROPERTIES(MarchingCubeMeshGenerator)
        INIT_PROPERTY(CaseNumber)
    END_INIT_PROPERTIES()

    mConfiguration.SetIsIndexed(true);
    mConfiguration.SetMeshPrimitiveType(MeshConfiguration::TRIANGLE);

    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_NORMAL);
    mConfiguration.SetIsDynamic(true);
}

MarchingCubeMeshGenerator::~MarchingCubeMeshGenerator()
{
}

void MarchingCubeMeshGenerator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);
    unsigned int caseSignature = (unsigned int)(GetCaseNumber() < 0 ? 0 : (GetCaseNumber() > 256 ? 255 : GetCaseNumber() ));
    const CaseTable* caseTable = g3dTerrainSystemInstance->GetCaseTable();
    
    MeshDataRef meshData = GetData(); 
    meshData->AllocateVertexes(caseTable->GetEdgeCount());
    struct Vertex
    {
        Vec4 position;
        Vec3 normal;
    }; 

    const float* edgeCoords = caseTable->GetEdgeCoordinates();
    Vertex* stream = meshData->GetStream<Vertex>(0);
    for (int i = 0; i < caseTable->GetEdgeCount(); ++i)
    {
        stream[i].position = Vec4(edgeCoords[3*i], edgeCoords[3*i + 1], edgeCoords[3*i + 2], 1.0f); 
        stream[i].normal = Vec3(0.0f);
    }

    const Case& caseEl = caseTable->GetCase(caseSignature);
    meshData->AllocateIndexes(caseEl.triangleCount * 3);
    unsigned short* idx = meshData->GetIndexBuffer();
    for (int i = 0; i < caseEl.triangleCount*3; ++i)
    {
        idx[i] = caseEl.triangles[i];
    }
    
    //compute normal data
    for (int i = 0; i < caseTable->GetEdgeCount(); ++i)
    {
        Vec3 normal(0.0f);
        bool found = false;
        for (int t = 0; t < caseEl.triangleCount; ++t)
        {
            char edgeIdA = caseEl.triangles[t*3];
            char edgeIdB = caseEl.triangles[t*3 + 1];
            char edgeIdC = caseEl.triangles[t*3 + 2];
            if (edgeIdA == i || edgeIdB == i || edgeIdC == i)
            {
                found = true;
                Vec3 A(&edgeCoords[edgeIdA * 3]);
                Vec3 B(&edgeCoords[edgeIdB * 3]);
                Vec3 C(&edgeCoords[edgeIdC * 3]);
                normal += Cross(B - A, C - A);
            }
        }
        if (found) Normalize(normal);
        stream[i].normal = normal;
    }
    
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

