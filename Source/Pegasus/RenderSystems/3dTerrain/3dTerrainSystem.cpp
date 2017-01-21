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
#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/RenderSystems/3dTerrain/MarchingCubeMeshGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/Terrain3dGenerator.h"
#include "Pegasus/RenderSystems/3dTerrain/Terrain3d.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Core/Formats.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Mesh/Generator/CustomGenerator.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/Math/Vector.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

using namespace Pegasus;
using namespace Pegasus::Shader;
using namespace Pegasus::Render;
using namespace Pegasus::Mesh;
using namespace Pegasus::Math;
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

    //mesh density inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::DENSITY_CS], "BlockStateCbuffer", mDensityBlockStateUniform);

    //compute  count / caseId inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS], "densityTexture", mGeomInfoDensityInput);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS], "countInfoCbuffer", mGeomInfoCaseCountInfoInput);

    //compute sparse inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::SPARSE_2_8_CS], "volumeCount", mVolumeCountUniform);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::SPARSE_1_4_CS], "sparse8", mSparse8Uniform);

    //mesh prod inputs
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "BlockStateCbuffer", mMeshBlockStateUniform);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "densityTexture", mMeshDensity);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshCounts",  mMeshCounts);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshSparse8", mMeshSparse8);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "meshSparse4", mMeshSparse4);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "countInfoCbuffer", mMeshInfoCaseCountInfoInput);
    Render::GetUniformLocation(mPrograms[Terrain3dSystem::MESH_PRODUCER_CS], "indexInfoCbuffer", mMeshInfoCaseIndices);
    
    mDensityInputBuffer = Render::CreateUniformBuffer(sizeof(Math::Vec4)*2);
    Render::SamplerStateConfig samplerConfig;
    samplerConfig.mFilter = Render::SamplerStateConfig::BILINEAR;
    mDensitySampler = Render::CreateSamplerState(samplerConfig);



#if PEGASUS_ENABLE_PROXIES
    LoadDebugObjects(appContext);
#endif
}

void Terrain3dSystem::CreateResources(TerrainResources& resources) const
{
    Render::VolumeTextureConfig volumeTextureConfig;
    
    //the terrain volume encompasses the 8 sample points each voxel has (every corner).
    //+2 comes from the 2 extra sample points needed to create a right side thick voxel. The +1 comes from the extra offset starting on the left to just store density, for normal computation.
    volumeTextureConfig.mWidth  = THREAD_DIM + 3;
    volumeTextureConfig.mHeight = THREAD_DIM + 3;
    volumeTextureConfig.mDepth  = THREAD_DIM + 3;

    volumeTextureConfig.mFormat = Core::FORMAT_R8_UNORM;
    resources.densityTexture = Render::CreateVolumeTexture(volumeTextureConfig);

    //index count and caseId per voxel, not per corner. The +1 is for the cap on the right most edge.
    volumeTextureConfig.mWidth  = THREAD_DIM + 1;
    volumeTextureConfig.mHeight = THREAD_DIM + 1;
    volumeTextureConfig.mDepth  = THREAD_DIM + 1;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.geoInfo = Render::CreateVolumeTexture(volumeTextureConfig);
    
    volumeTextureConfig.mWidth  = THREAD_DIM;
    volumeTextureConfig.mHeight = THREAD_DIM;
    volumeTextureConfig.mDepth  = THREAD_DIM;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.sparse8 = Render::CreateVolumeTexture(volumeTextureConfig);

    volumeTextureConfig.mWidth  = GROUP_DIM;
    volumeTextureConfig.mHeight = GROUP_DIM;
    volumeTextureConfig.mDepth  = GROUP_DIM;
    volumeTextureConfig.mFormat = Core::FORMAT_R16_UINT;
    resources.sparse4 = Render::CreateVolumeTexture(volumeTextureConfig);
}

void Terrain3dSystem::ComputeTerrainMesh(TerrainResources& resources, Render::BufferRef indexBuffer, Render::BufferRef vertexPosBuffer, Render::BufferRef vertexNormalBuffer, Render::BufferRef drawIndirectBufferArgs)
{
    //Set all constant buffers

    Render::SetBuffer(mDensityInputBuffer, &resources.blockState, sizeof(resources.blockState));

    // Generate terrain density
    Render::SetProgram(mPrograms[Terrain3dSystem::DENSITY_CS]);
    Render::SetUniformBuffer(mDensityBlockStateUniform, mDensityInputBuffer);
    Render::SetComputeOutput(resources.densityTexture, 0);    
    Render::Dispatch(GROUP_DIM,GROUP_DIM,GROUP_DIM);    
    Render::UnbindComputeOutputs();

    // compute geometry information.
    Render::SetProgram(mPrograms[Terrain3dSystem::GEOMETRY_INFO_CS]);
    Render::SetUniformVolume(mGeomInfoDensityInput, resources.densityTexture);
    Render::SetUniformBuffer(mGeomInfoCaseCountInfoInput, mPackedCaseCountInfoBuffer);
    Render::SetComputeOutput(resources.geoInfo , 0);
    Render::Dispatch(GROUP_DIM,GROUP_DIM,GROUP_DIM);    
    Render::UnbindComputeOutputs();

    // compute sparse offsets, on 8x8x8 groups.
    Render::SetProgram(mPrograms[Terrain3dSystem::SPARSE_2_8_CS]);
    Render::SetUniformVolume(mVolumeCountUniform, resources.geoInfo);
    Render::SetComputeOutput(resources.sparse8, 0);
    Render::Dispatch(GROUP_DIM,GROUP_DIM,GROUP_DIM);    
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
    Render::SetUniformBuffer(mMeshBlockStateUniform, mDensityInputBuffer);
    Render::SetUniformBuffer(mMeshInfoCaseCountInfoInput, mPackedCaseCountInfoBuffer);
    Render::SetUniformBuffer(mMeshInfoCaseIndices, mPackedIndexCasesBuffer);
    Render::SetComputeOutput(indexBuffer, 0);
    Render::SetComputeOutput(vertexPosBuffer, 1);
    Render::SetComputeOutput(vertexNormalBuffer, 2);
    Render::SetComputeOutput(drawIndirectBufferArgs, 3);
    Render::SetComputeSampler(mDensitySampler, 0);
    Render::Dispatch(GROUP_DIM,GROUP_DIM,GROUP_DIM);
    Render::UnbindComputeResources();
    Render::UnbindComputeOutputs();
}

void Terrain3dSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    Utils::Vector<BlockScript::FunctionDeclarationDesc> methods;
    Utils::Vector<BlockScript::FunctionDeclarationDesc> functions;
    Terrain3d::GetBlockScriptApi(methods, functions);

    //Include references to the camera render system.
    RenderSystemManager* renderSystemManager = appContext->GetRenderSystemManager();
    const RenderSystemManager::RenderSystemContainer* camSystem = renderSystemManager->FindSystem("CameraSystem");
    PG_ASSERT(camSystem != nullptr);
    blocklib->GetSymbolTable()->RegisterChild(camSystem->blockscriptLib->GetSymbolTable()); 

    //register the terrain type as a generic resource. This will make it accessible in blockscript!
    Application::GenericResource::RegisterGenericResourceType(Terrain3d::GetStaticClassInfo(), blocklib,  methods.Data(), methods.GetSize());

    blocklib->CreateIntrinsicFunctions(functions.Data(), functions.GetSize());

    blocklib->GetSymbolTable()->UnregisterChild(camSystem->blockscriptLib->GetSymbolTable()); 


}

void Terrain3dSystem::OnRegisterCustomMeshNodes(Pegasus::Mesh::MeshManager* meshManager)
{
    meshManager->RegisterMeshNode("MarchingCubeMeshGenerator", MarchingCubeMeshGenerator::CreateNode);
    meshManager->RegisterMeshNode("Terrain3dGenerator", Terrain3dGenerator::CreateNode);
}

#if PEGASUS_ENABLE_PROXIES
void Terrain3dSystem::RenderDebugBox(const Math::Vec3& minAabb, const Math::Vec3& maxAabb, const Math::Vec4& color)
{
    Vec3 offset = (maxAabb + minAabb) * 0.5f;
    Vec3 scale =  (maxAabb - minAabb) * 0.5f;

    struct {
        Vec4 offset;
        Vec4 scale;
        Vec4 color;
    } lodBlockState;

    lodBlockState.offset = Vec4(offset, 0.0f);
    lodBlockState.scale = Vec4(scale,  0.0f);
    lodBlockState.color = color;
    Render::SetBuffer(mBlockInputBuffer, &lodBlockState);
    Render::SetUniformBuffer(mBlockInput, mBlockInputBuffer);
    Render::Draw();
}

void Terrain3dSystem::LoadDebugObjects(Core::IApplicationContext* appContext)
{
    const char debugGridVsShader[] =
        "#include \"RenderSystems/Camera/Common.h\"\n"
        "cbuffer GridInput { float4 gPos; float4 gScale; float4 gGridCol; }\n"
        "void main(in float3 p0 : POSITION0,"
        "        out float3 outPixelPos : COLOR0,"
        "        out float4 outPos : SV_Position)"
        "{"
        "    outPixelPos = p0;"
        "    float3 pixelPos = p0*gScale.xyz + gPos.xyz;"
        "    outPos = mul(float4(pixelPos,1.0),gViewProj);"
        "}"
    ;
    const char debugGridPsShader[] =
        "cbuffer GridInput { float4 gPos; float4 gScale; float4 gGridCol; }\n"
        "float4 main(in float4 inPixelPos : COLOR0) : SV_Target\n"
        "{"
        "    float4 isColor = any(inPixelPos <= 0.01 || (inPixelPos >= 15.99));"
        "    return lerp(gGridCol, float4(0.2,1.0,0.0,gGridCol.a), isColor);"
        "}"
    ;

    const char debugBlockVsShader[] = 
        "#include \"RenderSystems/Camera/Common.h\"\n"
        "cbuffer BlockInput { float4 gOffset; float4 gScale; float4 gColor; }"
        "void main(in float4 p0 : POSITION, out float4 outPos : SV_Position) {"
        "   outPos = mul(float4(p0.xyz*gScale.xyz + gOffset.xyz,1.0), gViewProj);"
        "}"
    ;

    const char debugBlockPsShader[] =
        "cbuffer BlockInput { float4 gOffset; float4 gScale; float4 gColor; }"
        "float4 main() : SV_Target { return saturate(gColor); }"
    ;

    Pegasus::Shader::ShaderManager* sm = appContext->GetShaderManager();
    Pegasus::Mesh::MeshManager* mm = appContext->GetMeshManager();

    //Grid programs.
    {
        Pegasus::Shader::ShaderStageRef vsShader = sm->CreateShader();
        vsShader->SetSource(Pegasus::Shader::VERTEX, debugGridVsShader, sizeof(debugGridVsShader) / sizeof(debugGridVsShader[0]));
        Pegasus::Shader::ShaderStageRef psShader = sm->CreateShader();
        psShader->SetSource(Pegasus::Shader::FRAGMENT, debugGridPsShader, sizeof(debugGridPsShader) / sizeof(debugGridPsShader[0]));
        mGridProgram = sm->CreateProgram();
        mGridProgram->SetShaderStage(vsShader);
        mGridProgram->SetShaderStage(psShader);
        
        Render::GetUniformLocation(mGridProgram, "GridInput", mGridInput);
        mGridInputBuffer = Render::CreateUniformBuffer(16*3);
    }

    //Block programs.
    {
        Pegasus::Shader::ShaderStageRef vsShader = sm->CreateShader();
        vsShader->SetSource(Pegasus::Shader::VERTEX, debugBlockVsShader, sizeof(debugBlockVsShader) / sizeof(debugBlockVsShader[0]));
        Pegasus::Shader::ShaderStageRef psShader = sm->CreateShader();
        psShader->SetSource(Pegasus::Shader::FRAGMENT, debugBlockPsShader, sizeof(debugBlockPsShader) / sizeof(debugBlockPsShader[0]));
        mBlockProgram = sm->CreateProgram();
        mBlockProgram->SetShaderStage(vsShader);
        mBlockProgram->SetShaderStage(psShader);
        
        Render::GetUniformLocation(mBlockProgram, "BlockInput", mBlockInput);
        mBlockInputBuffer = Render::CreateUniformBuffer(16*3);
    }

    //Grid meshes.
    {
        mGridMesh = mm->CreateMeshNode();
        MeshGeneratorRef meshGenerator = mm->CreateMeshGeneratorNode("CustomGenerator");
        CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*meshGenerator));
        
        MeshConfiguration meshConfig;
        meshConfig.SetIsIndexed(true);
        meshConfig.SetIsDynamic(false);
        meshConfig.SetMeshPrimitiveType(Pegasus::Mesh::MeshConfiguration::LINE);
        MeshInputLayout* il = meshConfig.GetInputLayout();
        MeshInputLayout::AttrDesc posDesc;
        posDesc.mSemantic = MeshInputLayout::POSITION;
        posDesc.mType = Pegasus::Core::FORMAT_RGB_32_FLOAT;
        posDesc.mByteSize = 12;
        posDesc.mByteOffset = 0;
        posDesc.mSemanticIndex = 0;
        posDesc.mStreamIndex = 0;
        il->RegisterAttribute(posDesc);
        customGenerator->SetConfiguration(meshConfig);
        MeshDataRef meshData = customGenerator->EditMeshData();
        
        const unsigned int voxelUnits = Terrain3d::SEGMENT_UNIT_SIZE;
        float yLen = 0.0f;
        unsigned short iCount = 0;
        for (unsigned int i = 0; i <= voxelUnits; ++i)
        {
            float zLen = 0.0f;
            for (unsigned int j = 0; j <= voxelUnits; ++j)
            {
                Vec3 beginX(0.0f, yLen, zLen);
                Vec3 endX((float)voxelUnits, yLen, zLen);
                meshData->PushVertex<Vec3>(beginX,0);
                meshData->PushVertex<Vec3>(endX,0);
                meshData->PushIndex(iCount++);
                meshData->PushIndex(iCount++);

                Vec3 beginY(zLen, yLen, 0.0f);
                Vec3 endY(zLen, yLen, (float)voxelUnits);
                meshData->PushVertex<Vec3>(beginY,0);
                meshData->PushVertex<Vec3>(endY,0);
                meshData->PushIndex(iCount++);
                meshData->PushIndex(iCount++);

                Vec3 beginZ(zLen, 0.0f, yLen);
                Vec3 endZ(zLen, (float)voxelUnits, yLen);
                meshData->PushVertex<Vec3>(beginZ,0);
                meshData->PushVertex<Vec3>(endZ,0);
                meshData->PushIndex(iCount++);
                meshData->PushIndex(iCount++);

                zLen += 1.0f;
            }
            yLen += 1.0f; 
        }

        mGridMesh->SetGeneratorInput(customGenerator);
        
    }

    //Block meshes
    {
        mBlockMesh = mm->CreateMeshNode();

        MeshGeneratorRef meshGenerator = mm->CreateMeshGeneratorNode("CustomGenerator");
        CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*meshGenerator));
        
        MeshConfiguration meshConfig;
        meshConfig.SetIsIndexed(true);
        meshConfig.SetIsDynamic(false);
        meshConfig.SetMeshPrimitiveType(Pegasus::Mesh::MeshConfiguration::LINE);
        MeshInputLayout* il = meshConfig.GetInputLayout();
        MeshInputLayout::AttrDesc posDesc;
        posDesc.mSemantic = MeshInputLayout::POSITION;
        posDesc.mType = Pegasus::Core::FORMAT_RGBA_32_FLOAT;
        posDesc.mByteSize = 16;
        posDesc.mByteOffset = 0;
        posDesc.mSemanticIndex = 0;
        posDesc.mStreamIndex = 0;
        il->RegisterAttribute(posDesc);
        customGenerator->SetConfiguration(meshConfig);

        MeshDataRef meshData = customGenerator->EditMeshData();
        meshData->PushVertex(Vec4(-1.0f,  1.0f, -1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(1.0f,   1.0f, -1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(1.0f,  -1.0f, -1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(-1.0f, -1.0f, -1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(-1.0f,  1.0f, 1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(1.0f,   1.0f, 1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(1.0f,  -1.0f, 1.0f, 1.0f),0);
        meshData->PushVertex(Vec4(-1.0f, -1.0f, 1.0f, 1.0f),0);

        meshData->PushIndex(0);
        meshData->PushIndex(1);
        meshData->PushIndex(1);
        meshData->PushIndex(2);
        meshData->PushIndex(2);
        meshData->PushIndex(3);
        meshData->PushIndex(3);
        meshData->PushIndex(0);

        meshData->PushIndex(4);
        meshData->PushIndex(5);
        meshData->PushIndex(5);
        meshData->PushIndex(6);
        meshData->PushIndex(6);
        meshData->PushIndex(7);
        meshData->PushIndex(7);
        meshData->PushIndex(4);

        meshData->PushIndex(0);
        meshData->PushIndex(4);
        meshData->PushIndex(4);
        meshData->PushIndex(7);
        meshData->PushIndex(7);
        meshData->PushIndex(3);
        meshData->PushIndex(3);
        meshData->PushIndex(0);

        meshData->PushIndex(1);
        meshData->PushIndex(5);
        meshData->PushIndex(5);
        meshData->PushIndex(6);
        meshData->PushIndex(6);
        meshData->PushIndex(2);
        meshData->PushIndex(2);
        meshData->PushIndex(1);

        mBlockMesh->SetGeneratorInput(customGenerator);
    }

    //Grid raster states
    {
        Render::RasterizerConfig rc;        
        rc.mDepthFunc = Render::RasterizerConfig::LESSER_DF;
        mGridStateOutside = Render::CreateRasterizerState(rc);

        BlendingConfig bc;
        bc.mBlendingOperator = Render::BlendingConfig::NONE_BO;
        mNoBlending = Render::CreateBlendingState(bc);

        bc.mBlendingOperator = Render::BlendingConfig::ADD_BO;
        bc.mSource = Render::BlendingConfig::SRC_ALPHA;
        bc.mDest = Render::BlendingConfig::INV_SRC_ALPHA;
        mAlphaBlending = Render::CreateBlendingState(bc);


    }
    
}

void Terrain3dSystem::DrawDebugTerrain(Terrain3d* terrain)
{
    struct GridState
    {
        Vec4 pos;
        Vec4 scale;
        Vec4 col;
    } gridState;
    if (mEnableGeoDebug)
    {
        const Vec4 colInside(0.3f,0.8f,0.2f,0.2f); 
        const Vec4 colOutside(0.0f,0.4f,1.0f,0.5f); 
        int segmentListSize = 0;
        const Terrain3d::Segment* segmentList = terrain->GetCullResults(segmentListSize);
        static const int MAX_DRAW_CALLS = 300;
        if (segmentListSize > MAX_DRAW_CALLS)
        {
            PG_LOG('ERR_', "Exceeding number of draw calls for terrain debug. Clamping to 2000.");
            segmentListSize = MAX_DRAW_CALLS;
        }

        //! DEBUG PASS 1: draw camera cull results. These will represent mesh dispatches
        Render::SetBlendingState(mAlphaBlending);
        Render::SetRasterizerState(mGridStateOutside);
        Render::SetProgram(mGridProgram);
        Render::SetMesh(mGridMesh);
        Render::SetUniformBuffer(mGridInput, mGridInputBuffer);
        for (int s = 0; s < segmentListSize; ++s)
        {
            const Terrain3d::Segment& seg = segmentList[s];

            gridState.pos = Math::Vec4((float)seg.offset[0],(float)seg.offset[1],(float)seg.offset[2],0.0f);
            gridState.scale = Math::Vec4((float)seg.size[0],(float)seg.size[1],(float)seg.size[2],0.0f) * (float)seg.sizeResolution;

            //render outside 
            gridState.col = colOutside;
            Render::SetBuffer(mGridInputBuffer, &gridState);
            Draw();

        }

        //! DEBUG PASS 2: draw LOD boxes surrounding camera
        Render::SetBlendingState(mNoBlending);
        static const Vec4 sLodCols[Terrain3d::MAXIMUM_LODS] = {
            Vec4(1.0f,0.0f,0.0f,1.0f),
            Vec4(0.0f,1.0f,0.0f,1.0f),
            Vec4(0.0f,0.0f,1.0f,1.0f)
        };
        Render::SetProgram(mBlockProgram);
        Render::SetMesh(mBlockMesh);
        Render::SetRasterizerState(mGridStateOutside);

        // render now full lod boxes of terrain.
        for (unsigned int lod = 0; lod < Terrain3d::MAXIMUM_LODS; ++lod)
        {
            const Terrain3d::Box& lodBox = terrain->GetLodBoundingBox(lod);
            RenderDebugBox(lodBox.minAabb,lodBox.maxAabb,sLodCols[lod]);
        }

        //! DEBUG PASS 3: draw LOD cover boxes terrain. The intersection of these and the frustum generate the LOD boxes.
        for (unsigned int coverBoxId = 0; coverBoxId < Terrain3d::MAXIMUM_TERRAIN_COVER_BOXES; ++coverBoxId)
        {
            const Terrain3d::Segment& coverBox = terrain->GetLodCoverBox(coverBoxId);
            Vec3 minAabb = Vec3((float)coverBox.offset[0],(float)coverBox.offset[1],(float)coverBox.offset[2]);
            Vec3 maxAabb = minAabb + ((float)(coverBox.sizeResolution*Terrain3d::SEGMENT_UNIT_SIZE))*Vec3((float)coverBox.size[0],(float)coverBox.size[1],(float)coverBox.size[2]);
            Vec4 coverBoxCol(0.3f,0.1f,0.4f,1.0f);
            RenderDebugBox(minAabb,maxAabb,coverBoxCol);
        }

            
    }
    
}
#endif

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
