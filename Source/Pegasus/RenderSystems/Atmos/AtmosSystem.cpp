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
#include "Pegasus/RenderSystems/Atmos/BasicSky.h"
#include "Pegasus/RenderSystems/System/RenderSystemManager.h"
#include "Pegasus/RenderSystems/System/LutLib.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Core/Formats.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Texture/Generator/TexCustomGenerator.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Memory/MemoryManager.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

using namespace Pegasus;
using namespace Pegasus::Core;
using namespace Pegasus::Shader;
using namespace Pegasus::Render;
using namespace Pegasus::Mesh;
using namespace Pegasus::Texture;
using namespace Pegasus::Math;
using namespace Pegasus::RenderSystems;

AtmosSystem* gAtmosSystemInstance = nullptr;

#define ATMOS_PATH "RenderSystems/Atmos/"

static const char* sPrograms[] =
{
    ATMOS_PATH "BasicSky.pas"
};

void AtmosSystem::Load(Core::IApplicationContext* appContext)
{
    PG_ASSERTSTR(gAtmosSystemInstance == nullptr, "Can only have 1 instance of the atmos system.");
    gAtmosSystemInstance = this;
    MeshManager* meshManager = appContext->GetMeshManager();
    mQuadMesh = meshManager->CreateMeshNode();
    mQuadMesh->SetGeneratorInput(meshManager->CreateMeshGeneratorNode("QuadGenerator"));

    ShaderManager* shaderManager = appContext->GetShaderManager();

    for (unsigned int p = 0; p < PROGRAMS_COUNT; ++p)
    {
        Shader::ProgramLinkageRef program = shaderManager->LoadProgram(sPrograms[p]);
        PG_ASSERT(program != nullptr);
        mPrograms[p] = program;
    }

    RasterizerConfig skyRasterConfig;
    skyRasterConfig.mStencilFunc = RasterizerConfig::EQUAL_DF;
    skyRasterConfig.mStencilReadMask = 0x1;
    mSkyRasterState = CreateRasterizerState(skyRasterConfig);

    skyRasterConfig.mStencilFunc = RasterizerConfig::NONE_DF;
    mSkyRasterStateNoStencil = CreateRasterizerState(skyRasterConfig);

    //Basic Sky Resource loading
    GetUniformLocation(mPrograms[BASIC_SKY], "Inputs", mBasicSkyCbufferUniform);
    GetUniformLocation(mPrograms[BASIC_SKY], "acosLutTex", mBasicSkyAcosLutUniform);
    mBasicSkyCbuffer = CreateUniformBuffer(sizeof(BasicSkyCbuffer));

    SamplerStateConfig bilinearFilterConfig;
    bilinearFilterConfig.mFilter = SamplerStateConfig::BILINEAR;
    mBilinearFilter = CreateSamplerState(bilinearFilterConfig);

    GenerateCubeCams();

    mAcosLut = appContext->GetRenderSystemManager()->GetLutLib()->GetLut(LutLib::ACOS);
}


void AtmosSystem::GenerateCubeCams()
{
    static const Vec3 camDirs[] = {
        Vec3(-1.0f, 0.0f ,0.0f),
        Vec3(1.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        Vec3(0.0f, -1.0f, 0.0f),
        Vec3(0.0f, 0.0f, 1.0f),
        Vec3(0.0f, 0.0f, -1.0f)
    };
    for (unsigned i = 0; i < Render::CUBE_FACE_COUNT; ++i)
    {
        Pegasus::Camera::CameraRef faceCam = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "Camera alloc", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Camera::Camera(Pegasus::Memory::GetRenderAllocator());
        faceCam->SetDir(camDirs[i]);
        faceCam->SetNear(256);
        faceCam->SetFar(512);
        faceCam->SetAspect(1.0f);
        faceCam->SetEnableOrtho(1);
        faceCam->Update();
#if PEGASUS_ENABLE_PROXIES
        faceCam->SetShowCameraOnDebugMode(false);
#endif
        mCubeCams[i] = faceCam;
    }
}

void AtmosSystem::OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext)
{
    Utils::Vector<BlockScript::FunctionDeclarationDesc> methods;
    Utils::Vector<BlockScript::FunctionDeclarationDesc> functions;
    BasicSky::GetBlockScriptApi(methods, functions);
    Application::GenericResource::RegisterGenericResourceType(BasicSky::GetStaticClassInfo(), blocklib, methods.Data(), methods.GetSize());
    blocklib->CreateIntrinsicFunctions(functions.Data(), functions.GetSize());
}

void AtmosSystem::DrawBasicSky(BasicSky* basicSky, bool enableStencil)
{
    BeginMarker("BasicSky");
    SetProgram(mPrograms[BASIC_SKY]);
    SetMesh(mQuadMesh);
    BasicSkyCbuffer cbuffer;
    cbuffer.distance = basicSky->GetDistance();
    SetBuffer(mBasicSkyCbuffer, &cbuffer);
    SetUniformBuffer(mBasicSkyCbufferUniform, mBasicSkyCbuffer);
    SetUniformTexture(mBasicSkyAcosLutUniform,mAcosLut);
    SetPixelSampler(mBilinearFilter, 0);
    SetRasterizerState(enableStencil ? mSkyRasterState : mSkyRasterStateNoStencil, 0x0);
    Draw();
    EndMarker();
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
