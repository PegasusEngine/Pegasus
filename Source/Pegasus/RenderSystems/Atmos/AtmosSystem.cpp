/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.cpp
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  

#include "Pegasus/RenderSystems/Atmos/AtmosSystem.h"
#include "Pegasus/RenderSystems/Atmos/Atmosphere.h"
#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/RenderSystems/Volumes/MarchingCubeMeshGenerator.h"
#include "Pegasus/RenderSystems/Volumes/Terrain3dGenerator.h"
#include "Pegasus/RenderSystems/Volumes/Terrain3d.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Core/Formats.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Mesh/Generator/CustomGenerator.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/Math/Vector.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

using namespace Pegasus;
using namespace Pegasus::Shader;
using namespace Pegasus::Render;
using namespace Pegasus::Mesh;
using namespace Pegasus::Math;
using namespace Pegasus::RenderSystems;

AtmosSystem* gAtmosSystemInstance = nullptr;
#define ATMOS_PATH "RenderSystems/Atmos/"


//list of assets
static const char* sPrograms[AtmosSystem::PROGRAM_COUNT] =
{
	"GENERATE_ATMOS_LOOK_UP.hlsl"
};


void AtmosSystem::Load(Core::IApplicationContext* appContext)
{

	/*
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

	*/
}

void AtmosSystem::CreateResources(AtmosResources& resources) const
{
	/*

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

	*/
}

void AtmosSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
	/*

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
	*/

}

void AtmosSystem::OnRegisterCustomMeshNodes(Pegasus::Mesh::MeshManager* meshManager)
{
    //meshManager->RegisterMeshNode("MarchingCubeMeshGenerator", MarchingCubeMeshGenerator::CreateNode);
    //meshManager->RegisterMeshNode("Terrain3dGenerator", Terrain3dGenerator::CreateNode);
}

#if PEGASUS_ENABLE_PROXIES
void AtmosSystem::RenderDebugBox(const Math::Vec3& minAabb, const Math::Vec3& maxAabb, const Math::Vec4& color)
{
	/*
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
	*/
}

void AtmosSystem::LoadDebugObjects(Core::IApplicationContext* appContext)
{
	/*
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
    */
}

void AtmosSystem::DrawDebugAtmosphere(Atmosphere* atmosphere)
{

	/*
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
    */
}
#endif

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
