#include "Pegasus/RenderSystems/Volumes/Terrain3dGenerator.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_VOLUMES

#define PEGASUS_TERRAIN_3D_MESH_GENERATOR

#include "Pegasus/Mesh/IMeshFactory.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Utils/Memset.h"

using namespace Pegasus;
using namespace Pegasus::Mesh;
using namespace Pegasus::RenderSystems;
using namespace Pegasus::Math;

extern RenderSystems::VolumesSystem* gVolumesSystem;


BEGIN_IMPLEMENT_PROPERTIES(Terrain3dGenerator)
    IMPLEMENT_PROPERTY(Terrain3dGenerator, TerrainSeed)
END_IMPLEMENT_PROPERTIES(Terrain3dGenerator)

Terrain3dGenerator::Terrain3dGenerator(Pegasus::Alloc::IAllocator* nodeAllocator,
                          Pegasus::Alloc::IAllocator* nodeDataAllocator) : MeshGenerator(nodeAllocator, nodeDataAllocator), mIsComputeResourcesAllocated(false)
{
    BEGIN_INIT_PROPERTIES(Terrain3dGenerator)
        INIT_PROPERTY(TerrainSeed)
    END_INIT_PROPERTIES()

    mConfiguration.SetIsIndexed(true);
    mConfiguration.SetIsDynamic(false);//not dynamic, we dont push data through the CPU
    mConfiguration.SetIsDrawIndirect(true);//draw parameters are filled by internal compute shaders.
    mConfiguration.SetMeshPrimitiveType(MeshConfiguration::TRIANGLE);

    Mesh::MeshInputLayout inputLayout;

    Mesh::MeshInputLayout::AttrDesc posDesc;
    posDesc.mSemantic = Mesh::MeshInputLayout::POSITION;
    posDesc.mType = Pegasus::Core::FORMAT_RGB_32_FLOAT;
    posDesc.mByteSize = 12;
    posDesc.mByteOffset = 0;
    posDesc.mSemanticIndex = 0;
    posDesc.mStreamIndex = 0;
    inputLayout.RegisterAttribute(posDesc);

    Mesh::MeshInputLayout::AttrDesc normDesc;
    normDesc.mSemantic = Mesh::MeshInputLayout::NORMAL;
    normDesc.mType = Pegasus::Core::FORMAT_RGB_32_FLOAT;
    normDesc.mByteSize = 12;
    normDesc.mByteOffset = 0;
    normDesc.mSemanticIndex = 0;
    normDesc.mStreamIndex = 1;
    inputLayout.RegisterAttribute(normDesc);

    mConfiguration.SetInputLayout(inputLayout);
    
    Utils::Memset32(mProgramVersions, 0x0, sizeof(mProgramVersions));
}

bool Terrain3dGenerator::Update()
{
    bool updated = MeshGenerator::Update();

    if (!updated)
    {
        for (unsigned int i = 0; i < VolumesSystem::PROGRAM_COUNT; ++i)
        {
            int newVersion = Render::GetProgramVersion(gVolumesSystem->GetProgram(static_cast<VolumesSystem::Programs>(i)));
            if (newVersion != mProgramVersions[i])
            {
                mProgramVersions[i] = newVersion;
                updated = true;
            }
        }
    }
    
    if (updated)
    {
        InvalidateData();
        bool dummy;
        //TODO: allow user to do this explictely.
        GetUpdatedData(dummy);//generate data immediately. We dont want to lazily 
        GetData()->Validate();
    }
    return updated;
}

void Terrain3dGenerator::CreateComputeTerrainResources()
{
    if (!IsDataAllocated())
    {
        CreateData();
    }

    PG_ASSERTSTR(mIsComputeResourcesAllocated == false, "Internal gpu data is not allocated.");
    MeshDataRef meshData = GetData(); 
    meshData->AllocateVertexes(VolumesSystem::VERTEX_SIZE);
    meshData->AllocateIndexes(VolumesSystem::INDEX_SIZE);
    
    GetFactory()->GenerateMeshGPUData(meshData);
    mVertexBuffer = Render::GetVertexBuffer(meshData, 0);
    mNormalBuffer = Render::GetVertexBuffer(meshData, 1);
    mIndexBuffer = Render::GetIndexBuffer(meshData);
    mDrawIndirectBuffer = Render::GetDrawIndirectBuffer(meshData);

    gVolumesSystem->CreateResources(mResources);

    mIsComputeResourcesAllocated = true;
}

void Terrain3dGenerator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::BEGIN);
    MeshDataRef meshData = GetData(); 
    if (!mIsComputeResourcesAllocated)
    {
        CreateComputeTerrainResources();
    }
    else
    {
        //this is so Mesh does not call GenerateMeshGpuData again
        meshData->ValidateGPUData();
    }
    gVolumesSystem->ComputeTerrainMesh(mResources, mIndexBuffer, mVertexBuffer, mNormalBuffer, mDrawIndirectBuffer);
    
    PEGASUS_EVENT_DISPATCH(this, MeshOperationEvent, MeshOperationEvent::END_SUCCESS);
}

void Terrain3dGenerator::SetOffsetScale(float scale, const Math::Vec3 offset)
{
    mResources.blockState.worldOffset = Math::Vec4(offset, 0.0f);
    mResources.blockState.worldScale = Math::Vec4(scale, 0.0f,0.0f, 0.0f);
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
