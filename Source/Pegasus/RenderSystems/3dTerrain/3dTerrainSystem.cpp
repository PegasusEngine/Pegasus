/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"
#include "Pegasus/RenderSystems/3dTerrain/Terrain3d.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/RenderSystems/3dTerrain/MarchingCubeMeshGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/Terrain3dGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/Terrain3d.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/BlockScript/BlockLib.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

using namespace Pegasus;
using namespace Pegasus::RenderSystems;

Terrain3dSystem* g3dTerrainSystemInstance = nullptr;
#define TERRAIN3D_PATH "RenderSystems/3dTerrain/"


//list of assets
static const char* sPrograms[Terrain3dSystem::PROGRAM_COUNT] = 
{
    TERRAIN3D_PATH "density3d.cs",
    TERRAIN3D_PATH "sparse2_8.cs",
    TERRAIN3D_PATH "sparse1_4.cs",
    TERRAIN3D_PATH "geomInfo.cs",    
    TERRAIN3D_PATH "meshProducer.cs"
};


void Terrain3dSystem::Load(Core::IApplicationContext* appContext)
{
    mCaseTable.Initialize();
    PG_ASSERTSTR(g3dTerrainSystemInstance == nullptr, "Can only have 1 instance of the 3d terrain system.");
    g3dTerrainSystemInstance = this;

    for (unsigned int i = 0; i < PROGRAM_COUNT; ++i)
    {
        Shader::ProgramLinkageRef p = appContext->GetShaderManager()->CreateProgram();
        Shader::ShaderStageRef shadercs = appContext->GetShaderManager()->LoadShader(sPrograms[i]);
        PG_ASSERT(shadercs != nullptr);
        p->SetShaderStage(shadercs);
        mPrograms[i] = p;
    }

    // preparing resources
    mPackedCaseCountInfoBuffer = Render::CreateUniformBuffer(sizeof(unsigned int) * mCaseTable.GetCaseCount());
    Render::SetBuffer(mPackedCaseCountInfoBuffer, mCaseTable.GetPackedCaseCountInfoBuffer());

    mPackedIndexCasesBuffer = Render::CreateUniformBuffer(mCaseTable.GetPackedIndexCaseByteSize());
    Render::SetBuffer(mPackedIndexCasesBuffer, mCaseTable.GetPackedIndexCases());

    //compute  count / caseId inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS], "densityTexture", mGeomInfoDensityInput);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS], "countInfoCbuffer", mGeomInfoCaseCountInfoInput);

    //compute sparse inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::SPARSE_2_8_CS], "volumeCount", mVolumeCountUniform);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::SPARSE_1_4_CS], "sparse8", mSparse8Uniform);

    //mesh prod inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "densityTexture", mMeshDensity);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshCounts",  mMeshCounts);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshSparse8", mMeshSparse8);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshSparse4", mMeshSparse4);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "countInfoCbuffer", mMeshInfoCaseCountInfoInput);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "indexInfoCbuffer", mMeshInfoCaseIndices);
    
    Render::SamplerStateConfig samplerConfig;
    samplerConfig.mFilter = Render::SamplerStateConfig::BILINEAR;
    mDensitySampler = Render::CreateSamplerState(samplerConfig);
}

int Terrain3dSystem::GetVertexCount() const
{
    return 17*17*17*3;
}

int Terrain3dSystem::GetIndexCount() const
{
    return 8*8*8*5*3*8;
}

void Terrain3dSystem::CreateResources(TerrainResources& resources) const
{
    const unsigned int BlockSize = 16;
    Render::VolumeTextureConfig volumeTextureConfig;
    
    //the terrain volume encompasses the 8 sample points each voxel has (every corner).
    // voxel center for this case is considered to be at 0.5 xyz units. Meaning that the left most corner is 0.0, right most being 1.0 per voxel.
    volumeTextureConfig.mWidth  = BlockSize + 2;
    volumeTextureConfig.mHeight = BlockSize + 2;
    volumeTextureConfig.mDepth  = BlockSize + 2;

    volumeTextureConfig.mFormat = Core::FORMAT_R8_UNORM;
    resources.densityTexture = Render::CreateVolumeTexture(volumeTextureConfig);

    //index count and caseId per voxel, not per corner.
    volumeTextureConfig.mWidth  = BlockSize + 1;
    volumeTextureConfig.mHeight = BlockSize + 1;
    volumeTextureConfig.mDepth  = BlockSize + 1;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.geoInfo = Render::CreateVolumeTexture(volumeTextureConfig);
    
    volumeTextureConfig.mWidth  = BlockSize;
    volumeTextureConfig.mHeight = BlockSize;
    volumeTextureConfig.mDepth  = BlockSize;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.sparse8 = Render::CreateVolumeTexture(volumeTextureConfig);

    volumeTextureConfig.mWidth  /= 8;
    volumeTextureConfig.mHeight /= 8;
    volumeTextureConfig.mDepth  /= 8;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.sparse4 = Render::CreateVolumeTexture(volumeTextureConfig);
}

void Terrain3dSystem::RenderTerrain(TerrainResources& resources, Render::BufferRef indexBuffer, Render::BufferRef vertexPosBuffer, Render::BufferRef vertexNormalBuffer, Render::BufferRef drawIndirectBufferArgs)
{
    // Generate terrain density
    Render::SetProgram(mPrograms[Terrain3dSystem::DENSITY_CS]);
    Render::SetComputeOutput(resources.densityTexture, 0);    
    Render::Dispatch(2,2,2);    
    Render::UnbindComputeOutputs();

    // compute geometry information.
    Render::SetProgram(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS]);
    Render::SetUniformVolume(mGeomInfoDensityInput, resources.densityTexture);
    Render::SetUniformBuffer(mGeomInfoCaseCountInfoInput, mPackedCaseCountInfoBuffer);
    Render::SetComputeOutput(resources.geoInfo , 0);
    Render::Dispatch(2,2,2);
    Render::UnbindComputeOutputs();

    // compute sparse offsets, on 8x8x8 groups.
    Render::SetProgram(mPrograms[Terrain3dSystem::SPARSE_2_8_CS]);
    Render::SetUniformVolume(mVolumeCountUniform, resources.geoInfo);
    Render::SetComputeOutput(resources.sparse8, 0);
    Render::Dispatch(2,2,2);
    Render::UnbindComputeOutputs();
    
    // compute sparse offsets, on 2x2 groups.
    Render::SetProgram(mPrograms[Terrain3dSystem::SPARSE_1_4_CS]);
    Render::SetUniformVolume(mSparse8Uniform, resources.sparse8);    
    Render::SetComputeOutput(resources.sparse4, 0);
    Render::Dispatch(1,1,1);
    Render::UnbindComputeOutputs();

    // compute pass, write off mesh
    Render::SetProgram(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS]);
    Render::SetUniformVolume(mMeshDensity, resources.densityTexture);
    Render::SetUniformVolume(mMeshCounts, resources.geoInfo);
    Render::SetUniformVolume(mMeshSparse8, resources.sparse8);
    Render::SetUniformVolume(mMeshSparse4, resources.sparse4);
    Render::SetUniformBuffer(mMeshInfoCaseCountInfoInput, mPackedCaseCountInfoBuffer);
    Render::SetUniformBuffer(mMeshInfoCaseIndices, mPackedIndexCasesBuffer);
    Render::SetComputeOutput(indexBuffer, 0);
    Render::SetComputeOutput(vertexPosBuffer, 1);
    Render::SetComputeOutput(vertexNormalBuffer, 2);
    Render::SetComputeOutput(drawIndirectBufferArgs, 3);
    Render::SetComputeSampler(mDensitySampler, 0);
    Render::Dispatch(2,2,2);
    Render::UnbindComputeResources();
    Render::UnbindComputeOutputs();
    
}

void Terrain3dSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    Utils::Vector<BlockScript::FunctionDeclarationDesc> methods;
    Utils::Vector<BlockScript::FunctionDeclarationDesc> functions;
    Terrain3d::GetBlockScriptApi(methods, functions);

    //register the terrain type as a generic resource. This will make it accessible in blockscript!
    Application::GenericResource::RegisterGenericResourceType(Terrain3d::GetStaticClassInfo(), blocklib,  methods.Data(), methods.GetSize());

    blocklib->CreateIntrinsicFunctions(functions.Data(), functions.GetSize());


}

void Terrain3dSystem::OnRegisterCustomMeshNodes(Mesh::MeshManager* meshManager)
{
    meshManager->RegisterMeshNode("MarchingCubeMeshGenerator", MarchingCubeMeshGenerator::CreateNode);
    meshManager->RegisterMeshNode("Terrain3dGenerator", Terrain3dGenerator::CreateNode);
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
