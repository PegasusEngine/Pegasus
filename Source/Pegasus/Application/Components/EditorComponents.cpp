/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EditorComponents.cpp
//! \author	Kleber Garcia
//! \date   March 29th 2016
//! \brief	Application visual components for editor specific items (things that get edited)

#include "Pegasus/Application/Components/EditorComponents.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Shader/ShaderStage.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Mesh/Generator/CustomGenerator.h"
#include "Pegasus/Math/Constants.h"
#include "Pegasus/Math/Scalar.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"
#include "Pegasus/RenderSystems/Camera/Camera.h"
#include "Pegasus/RenderSystems/Lighting/LightingSystem.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS
#include "Pegasus/RenderSystems/Atmos/AtmosSystem.h"
#endif


#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN
#include "Pegasus/RenderSystems/3dTerrain/3dTerrainSystem.h"
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
#include "Pegasus/RenderSystems/Camera/CameraSystem.h"
#endif


#if PEGASUS_ENABLE_PROXIES

using namespace Pegasus;
using namespace Pegasus::App;
using namespace Pegasus::Mesh;
using namespace Pegasus::Render;
using namespace Pegasus::Math;
using namespace Pegasus::Camera;

BEGIN_IMPLEMENT_PROPERTIES(TextureViewComponentState)
    IMPLEMENT_PROPERTY(TextureViewComponentState, Offset)
    IMPLEMENT_PROPERTY(TextureViewComponentState, Scale)
END_IMPLEMENT_PROPERTIES(TextureViewComponentState)

TextureViewComponentState::TextureViewComponentState()
{
    
    BEGIN_INIT_PROPERTIES(TextureViewComponentState)
        INIT_PROPERTY(Offset)
        INIT_PROPERTY(Scale)
    END_INIT_PROPERTIES()

    mOnStartOffset = false;
    mOnStartScale = false;
    mPrevMousePos[0] = 0.0f;
    mPrevMousePos[1] = 0.0f;
    mMousePos[0] = 0.0f;
    mMousePos[1] = 0.0f;
}

TextureViewComponent::TextureViewComponent(Alloc::IAllocator* allocator)
: mAlloc(allocator)
{
}

TextureViewComponent::~TextureViewComponent()
{
}

Wnd::WindowComponentState* TextureViewComponent::CreateState(const Wnd::ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "New texture view state", Pegasus::Alloc::PG_MEM_PERM) TextureViewComponentState;
}

void TextureViewComponent::DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    PG_DELETE(mAlloc, state);
}

void TextureViewComponent::Load(Core::IApplicationContext* appContext)
{
    //! Shader declarations
    const char textureVsShader[] = 
        "void main(in float3 p0 : POSITION0,"
        "            in float2 t0 : TEXCOORD0,"
        "            out float2 outTexCoord : TEXTURE0,"
        "            out float4 outPos : SV_Position)"
        "{"
        "   outTexCoord = t0;"
        "   outPos = float4(p0, 1.0);"
        "}"
    ;

    const char texturePsShader[] = 
        "Texture2D targetTexture;"
        "SamplerState S"
        "{"
	    "    MipFilter = POINT;"
        "    MinFilter = POINT;"
        "    MagFilter = POINT;"
        "};"
        "float4 main(in float2 texCoord : TEXTURE0) : SV_Target"
        "{"
        "   return float4(targetTexture.Sample(S,texCoord).rgb,1.0);"
        "}"
    ;
    
    //! Setup renderer shader program
    Pegasus::Shader::ShaderStageRef vertexShader = appContext->GetShaderManager()->CreateShader();
    vertexShader->SetSource(Pegasus::Shader::VERTEX, textureVsShader, sizeof(textureVsShader));
    Pegasus::Shader::ShaderStageRef pixelShader = appContext->GetShaderManager()->CreateShader();
    pixelShader->SetSource(Pegasus::Shader::FRAGMENT, texturePsShader, sizeof(texturePsShader));
    mTextureRenderProgram = appContext->GetShaderManager()->CreateProgram();
    mTextureRenderProgram->SetShaderStage(vertexShader);
    mTextureRenderProgram->SetShaderStage(pixelShader);
    GetUniformLocation(mTextureRenderProgram, "targetTexture", mTargetTexUniform);

    mQuad = appContext->GetMeshManager()->CreateMeshNode();
    Pegasus::Mesh::MeshGeneratorRef quadGen = appContext->GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad->SetGeneratorInput(quadGen);

    //default raster / blend states
    RasterizerConfig rasterConfig;
    rasterConfig.mCullMode = RasterizerConfig::NONE_CM;
    rasterConfig.mDepthFunc = RasterizerConfig::NONE_DF;
    mRasterState =  CreateRasterizerState(rasterConfig);

    BlendingConfig blendConfig;
    blendConfig.mBlendingOperator = BlendingConfig::NONE_BO;
    mBlendState = CreateBlendingState(blendConfig);
}

void TextureViewComponent::Update(Core::IApplicationContext* appContext)
{
}

void TextureViewComponent::WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
}

void TextureViewComponent::Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    TextureViewComponentState* textureViewState = static_cast<TextureViewComponentState*>(state); 
    if (textureViewState->mTargetTexture != nullptr)
    {
        DispatchDefaultRenderTarget();
        Viewport vp(context.mTargetWindow->GetWidth(), context.mTargetWindow->GetHeight());
        SetViewport(vp);
        SetBlendingState(mBlendState);
        SetRasterizerState(mRasterState);
        SetClearColorValue(Pegasus::Math::ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
        Clear(true, false, false);
        SetProgram(mTextureRenderProgram);
        SetMesh(mQuad);
        SetUniformTexture(mTargetTexUniform, textureViewState->mTargetTexture);
        Draw();
    }
}

void TextureViewComponent::Unload(Core::IApplicationContext* appContext)
{
} 

void TextureViewComponent::OnMouseEvent(Wnd::WindowComponentState* state, IWindowComponent::MouseButton button, bool isDown, float x, float y)
{
    int i = 0;
}

void TextureViewComponent::OnKeyEvent(Wnd::WindowComponentState* state, char key, bool isDown)
{
    int i = 0;
}

BEGIN_IMPLEMENT_PROPERTIES(GridComponentState)
    IMPLEMENT_PROPERTY(GridComponentState, Scale)
    IMPLEMENT_PROPERTY(GridComponentState, EnableReticle)
    IMPLEMENT_PROPERTY(GridComponentState, EnableGrid)
    IMPLEMENT_PROPERTY(GridComponentState, InternalScale)
    IMPLEMENT_PROPERTY(GridComponentState, Len)
END_IMPLEMENT_PROPERTIES(GridComponentState)

GridComponentState::GridComponentState()
{
    BEGIN_INIT_PROPERTIES(GridComponentState)
        INIT_PROPERTY(Scale)
        INIT_PROPERTY(EnableReticle)
        INIT_PROPERTY(EnableGrid)
        INIT_PROPERTY(InternalScale)
        INIT_PROPERTY(Len)
    END_INIT_PROPERTIES()
}

GridComponent::GridComponent(Alloc::IAllocator* allocator)
: mAlloc(allocator)
{
}

GridComponent::~GridComponent()
{
}

Wnd::WindowComponentState* GridComponent::CreateState(const Wnd::ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "New texture view state", Pegasus::Alloc::PG_MEM_PERM) GridComponentState();
}

void GridComponent::DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
}

void GridComponent::Load(Core::IApplicationContext* appContext)
{

    //! Shader declarations
    const char gridVsShader[] = 
        "#include \"RenderSystems/Camera/Common.h\"\n"
        "void main(in float4 p0 : POSITION0,"
        "          in float4 c0 : COLOR0,"
        "          out float4 outCol : COLOR0,"
        "          out float4 outView : COLOR1,"
        "          out float4 outPos : SV_Position)"
        "{"
        "   outCol = c0;"
        "   outView = float4(gEyePos.xyz,0.0) - float4(p0.xyz,0.0);"
        "   outPos = mul(p0,gViewProj);;"
        "}"
    ;

    const char gridPsShader[] = 
        "float4 main(in float4 c0 : COLOR0, in float4 v : COLOR1) : SV_Target"
        "{"
        "   return saturate(c0+0.7*pow(saturate(1.0 - dot(normalize(v.xyz),float3(0.0,1.0,0.0))),4.0));"
        "}"
    ;

    const char reticleVsShader[] = 
        "#include \"RenderSystems/Camera/Common.h\"\n"
        "void main(in float4 p0 : POSITION0,"
        "          in float4 c0 : COLOR0,"
        "          out float4 outCol : COLOR0,"
        "          out float4 outPos : SV_Position)"
        "{"
        "   outCol = c0;"
        "   float4 camPos = mul(float4(0.2*p0.xyz,0.0),gView);"
        "   outPos = float4(camPos.x*gProj[0][0]/gProj[1][1],camPos.y, 0.1,1.0)+float4(0.9,-0.7,0.0,0.0);"
        "}"
    ;

    const char reticlePsShaderSrc[] = 
        "float4 main(in float4 c0 : COLOR0) : SV_Target"
        "{"
        "   return c0;"
        "}"
    ;


    
    //! Setup grid shader program
    Pegasus::Shader::ShaderStageRef vertexShader = appContext->GetShaderManager()->CreateShader();
    vertexShader->SetSource(Pegasus::Shader::VERTEX, gridVsShader, sizeof(gridVsShader));
    Pegasus::Shader::ShaderStageRef pixelShader = appContext->GetShaderManager()->CreateShader();
    pixelShader->SetSource(Pegasus::Shader::FRAGMENT, gridPsShader, sizeof(gridPsShader));
    mGridProgram = appContext->GetShaderManager()->CreateProgram();
    mGridProgram->SetShaderStage(vertexShader);
    mGridProgram->SetShaderStage(pixelShader);

    //! Setup reticle shader program
    Pegasus::Shader::ShaderStageRef reticleVertexShader = appContext->GetShaderManager()->CreateShader();
    reticleVertexShader->SetSource(Pegasus::Shader::VERTEX, reticleVsShader, sizeof(reticleVsShader));
    Pegasus::Shader::ShaderStageRef reticlePixelShader = appContext->GetShaderManager()->CreateShader();
    reticlePixelShader->SetSource(Pegasus::Shader::FRAGMENT, reticlePsShaderSrc, sizeof(reticlePsShaderSrc));
    mReticleProgram = appContext->GetShaderManager()->CreateProgram();
    mReticleProgram->SetShaderStage(reticleVertexShader);
    mReticleProgram->SetShaderStage(reticlePixelShader);

    RasterizerConfig rc;
    rc.mCullMode = RasterizerConfig::NONE_CM;
    rc.mDepthFunc = RasterizerConfig::LESSER_DF;
    mRasterState = CreateRasterizerState(rc);

    rc.mCullMode = RasterizerConfig::NONE_CM;
    rc.mDepthFunc = RasterizerConfig::NONE_DF;
    mReticleRasterState = CreateRasterizerState(rc);

    //! ************** BEGIN GRID MESH **************//
    //! Generator grid mesh
    MeshManager* mm = appContext->GetMeshManager();
    mGridGenerator = mm->CreateMeshGeneratorNode("CustomGenerator");
    mGrid = mm->CreateMeshNode();

    CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*mGridGenerator));
    MeshConfiguration meshConfig;
    MeshInputLayout il;
    meshConfig.SetIsIndexed(true);
    meshConfig.SetIsDynamic(false);
    meshConfig.SetMeshPrimitiveType(MeshConfiguration::LINE);
    MeshInputLayout::AttrDesc posDesc;    
    posDesc.mByteOffset = 0;
    posDesc.mSemantic = MeshInputLayout::POSITION;
    posDesc.mSemanticIndex = 0;
    posDesc.mStreamIndex = 0;
    posDesc.mType = Pegasus::Core::FORMAT_RGBA_32_FLOAT;
    posDesc.mByteSize = 16;
    il.RegisterAttribute(posDesc);

    MeshInputLayout::AttrDesc colDesc = posDesc;    
    colDesc.mType = Pegasus::Core::FORMAT_RGBA_8_UNORM;
    colDesc.mByteSize = 4;
    colDesc.mByteOffset = posDesc.mByteOffset + posDesc.mByteSize;
    colDesc.mSemantic = MeshInputLayout::COLOR;
    il.RegisterAttribute(colDesc);

    meshConfig.SetInputLayout(il);

    customGenerator->SetConfiguration(meshConfig);

    MeshDataRef meshData = customGenerator->EditMeshData();
    static const int GRID_EXTENDS = 200;
    int vertexCount = 4*(GRID_EXTENDS + 1);
    meshData->AllocateVertexes(vertexCount);
    meshData->AllocateIndexes(vertexCount);

    struct Vertex
    {
        Vec4 pos;
        Color8RGBA col;
    public:
        Vertex(Vec4& p, Color8RGBA& c) : pos(p), col(c) {}
    };

    Vertex * stream = meshData->GetStream<Vertex>(0);
    Color8RGBA gridCol = Color8RGBA(128,128,128,255);
    float lineLen = (float)GRID_EXTENDS;
    for (int i = 0; i <= GRID_EXTENDS; ++i)
    {
        Vec4 origin = Vec4(0.0f,0.0f,(float)i,1.0f) - Vec4(lineLen*0.5f,0.0f,lineLen*0.5f,0.0f);
        stream[4*i] = Vertex(origin, gridCol);
        stream[4*i + 1] = Vertex(origin + Vec4(lineLen, 0.0f, 0.0f, 0.0f), gridCol);

        Vec4 origin2 = Vec4(origin.z,origin.y,origin.x,1.0);
        stream[4*i + 2] = Vertex(origin2, gridCol);
        stream[4*i + 3] = Vertex(origin2 + Vec4(0.0f, 0.0f, lineLen, 0.0f), gridCol);
        PG_ASSERT(4*i+3 < vertexCount);
    }

    unsigned short * idx = meshData->GetIndexBuffer();
    for (unsigned short l = 0; l < (short)vertexCount; ++l)
    {
        idx[l] = l;
    }
    
    mGrid->SetGeneratorInput(mGridGenerator); //finish creation
     
    
    //! ************** BEGIN RETICLE MESH **************//

    mReticle = mm->CreateMeshNode();
    mReticleGenerator = mm->CreateMeshGeneratorNode("CustomGenerator");
    CustomGenerator* reticleCustomGen = static_cast<CustomGenerator*>(mReticleGenerator);
    reticleCustomGen->SetConfiguration(meshConfig);

    MeshDataRef reticleMeshData = reticleCustomGen->EditMeshData();

    // X axis
    Color8RGBA xColor(255,0,0,255);
    Color8RGBA yColor(0,255,0,255);
    Color8RGBA zColor(0,0,255,255);
    reticleMeshData->PushVertex(Vertex(Vec4(0.0f,0.0f,0.0f,1.0f), xColor),0);
    reticleMeshData->PushVertex(Vertex(Vec4(1.0f,0.0f,0.0f,1.0f), xColor),0);
    reticleMeshData->PushVertex(Vertex(Vec4(0.0f,0.0f,0.0f,1.0f), yColor),0);
    reticleMeshData->PushVertex(Vertex(Vec4(0.0f,1.0f,0.0f,1.0f), yColor),0);
    reticleMeshData->PushVertex(Vertex(Vec4(0.0f,0.0f,0.0f,1.0f), zColor),0);
    reticleMeshData->PushVertex(Vertex(Vec4(0.0f,0.0f,1.0f,1.0f), zColor),0);

    for (unsigned short i = 0; i < 6; ++i) reticleMeshData->PushIndex(i);

    mReticle->SetGeneratorInput(mReticleGenerator);

}

void GridComponent::Update(Core::IApplicationContext* appContext)
{
}

void GridComponent::WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
}

void GridComponent::Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{

    GridComponentState* gridState = static_cast<GridComponentState*>(state);
    DispatchDefaultRenderTarget();
    
    if (gridState->GetEnableGrid())
    {
        Render::BeginMarker("GridComponentGrid");
        SetProgram(mGridProgram);
        SetRasterizerState(mRasterState);    
        SetMesh(mGrid);
        Draw();
        Render::EndMarker();
    }

    
    if (gridState->GetEnableReticle())
    {
        Render::BeginMarker("GridComponentAxisReticle");
        SetProgram(mReticleProgram);
        SetRasterizerState(mReticleRasterState);
        SetMesh(mReticle); 
        Draw();
        Render::EndMarker();
    }

}

void GridComponent::Unload(Core::IApplicationContext* appContext)
{
}

#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA

namespace Pegasus
{
#if RENDER_SYSTEM_CONFIG_ENABLE_CAMERA
    namespace Camera
    {
        extern CameraSystem* gCameraSystem;
    }
#endif
}

BEGIN_IMPLEMENT_PROPERTIES(CameraDebugComponentState)
    IMPLEMENT_PROPERTY(CameraDebugComponentState, EnableDebug)
END_IMPLEMENT_PROPERTIES(CameraDebugComponentState)

CameraDebugComponentState::CameraDebugComponentState()
{
    BEGIN_INIT_PROPERTIES(CameraDebugComponentState)
        INIT_PROPERTY(EnableDebug)
    END_INIT_PROPERTIES()
}

Wnd::WindowComponentState* CameraDebugComponent::CreateState(const Wnd::ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "CameraDebugComponentState", Pegasus::Alloc::PG_MEM_PERM) CameraDebugComponentState();
}

void CameraDebugComponent::DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    PG_DELETE(mAlloc, state);
}

void CameraDebugComponent::Load(Core::IApplicationContext* appContext)
{
    //! Shader declarations
    const char vsShader[] = 
        "#include \"RenderSystems/Camera/Common.h\"\n"
        "cbuffer TargetCamState {"
        "   float4x4 gTargetCamInvViewProj;"
        "}"
        "void main(in float4 p0 : POSITION0,"
        "          out float4 outPos : SV_Position)"
        "{"
        "   float4 worldPos = mul(p0,gTargetCamInvViewProj);" 
        "   worldPos /= worldPos.w;" 
        "   outPos = mul(worldPos,gViewProj);"
        "}"
    ;

    const char psShader[] = 
        "float4 main() : SV_Target"
        "{"
        "   return float4(1.0,1.0,1.0,1.0);"
        "}"
    ;
    
    //! Setup renderer shader program
    Pegasus::Shader::ShaderStageRef vertexShader = appContext->GetShaderManager()->CreateShader();
    vertexShader->SetSource(Pegasus::Shader::VERTEX, vsShader, sizeof(vsShader));
    Pegasus::Shader::ShaderStageRef pixelShader = appContext->GetShaderManager()->CreateShader();
    pixelShader->SetSource(Pegasus::Shader::FRAGMENT, psShader, sizeof(psShader));
    mCamDebugProgram = appContext->GetShaderManager()->CreateProgram();
    mCamDebugProgram->SetShaderStage(vertexShader);
    mCamDebugProgram->SetShaderStage(pixelShader);

    MeshManager* mm = appContext->GetMeshManager();
    mCamMesh = mm->CreateMeshNode();
    mGenerator = mm->CreateMeshGeneratorNode("CustomGenerator");
    CustomGenerator* customGen = static_cast<CustomGenerator*>(mGenerator);
    MeshConfiguration meshConfig;
    meshConfig.SetIsIndexed(true);
    meshConfig.SetIsDynamic(false);
    meshConfig.SetMeshPrimitiveType(MeshConfiguration::LINE);
    MeshInputLayout camIl;
    camIl.GenerateEditorLayout(MeshInputLayout::USE_POSITION);
    meshConfig.SetInputLayout(camIl);
    customGen->SetConfiguration(meshConfig);
    MeshDataRef meshData = customGen->EditMeshData();
    meshData->PushVertex(Vec4(-1.0f,  1.0f, 0.0f, 1.0f),0);
    meshData->PushVertex(Vec4(1.0f,   1.0f, 0.0f, 1.0f),0);
    meshData->PushVertex(Vec4(1.0f,  -1.0f, 0.0f, 1.0f),0);
    meshData->PushVertex(Vec4(-1.0f, -1.0f, 0.0f, 1.0f),0);
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

    mCamMesh->SetGeneratorInput(mGenerator);

    RasterizerConfig rc;
    rc.mCullMode = RasterizerConfig::NONE_CM;
    rc.mDepthFunc = RasterizerConfig::NONE_DF;
    mDebugCamRasterState = CreateRasterizerState(rc);

    GetUniformLocation(mCamDebugProgram, "TargetCamState",mCamUniform);
    mCamUniformBuffer = CreateUniformBuffer(sizeof(Mat44));
}

void CameraDebugComponent::Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    CameraDebugComponentState* camState = static_cast<CameraDebugComponentState*>(state);
    if (camState->GetEnableDebug())
    {
        Render::BeginMarker("CameraDebugComponent");
        for (unsigned i = 0; i < Camera::Camera::GetCameraCount(); ++i)
        {
            Camera::Camera* debugCam = Camera::Camera::GetCameraInstance(i);
            if (debugCam->GetShowCameraOnDebugMode())
            {
                debugCam->Update();
                debugCam->WindowUpdate(context.mTargetWindow->GetWidth(), context.mTargetWindow->GetHeight());
                Camera::Camera::GpuCamData& gpuData = debugCam->GetGpuData();
                Mat44 invMat;
                Inverse(invMat, gpuData.viewProj);
                SetBuffer(mCamUniformBuffer,&invMat);
                SetProgram(mCamDebugProgram);
                SetUniformBuffer(mCamUniform, mCamUniformBuffer);
                SetMesh(mCamMesh);
                SetRasterizerState(mDebugCamRasterState);
                Draw();
            }
        }
        Render::EndMarker();
    }
}
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

extern Pegasus::RenderSystems::AtmosSystem* gAtmosSystemInstance;


#endif


#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN


extern Pegasus::RenderSystems::Terrain3dSystem* g3dTerrainSystemInstance;

BEGIN_IMPLEMENT_PROPERTIES(Terrain3dDebugComponentState)
    IMPLEMENT_PROPERTY(Terrain3dDebugComponentState, EnableDebugGeometry)
    IMPLEMENT_PROPERTY(Terrain3dDebugComponentState, EnableDebugCameraCull)
END_IMPLEMENT_PROPERTIES(Terrain3dDebugComponentState)

Terrain3dDebugComponentState::Terrain3dDebugComponentState()
{
    BEGIN_INIT_PROPERTIES(Terrain3dDebugComponentState)
        INIT_PROPERTY(EnableDebugGeometry)
        INIT_PROPERTY(EnableDebugCameraCull)
    END_INIT_PROPERTIES()
}

Wnd::WindowComponentState* Terrain3dDebugComponent::CreateState(const Wnd::ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "Terrain3dDebugComponentState", Pegasus::Alloc::PG_MEM_PERM) Terrain3dDebugComponentState();
}

void Terrain3dDebugComponent::DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    PG_DELETE(mAlloc, state);
}

void Terrain3dDebugComponent::WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    Terrain3dDebugComponentState* terrainState = static_cast<Terrain3dDebugComponentState*>(state);
    g3dTerrainSystemInstance->UpdateDebugState(terrainState->GetEnableDebugGeometry(),terrainState->GetEnableDebugCameraCull());
}
#endif

#if RENDER_SYSTEM_CONFIG_ENABLE_LIGHTING

extern Pegasus::RenderSystems::LightingSystem* gLightingSystemInstance;
BEGIN_IMPLEMENT_PROPERTIES(LightingDebugComponentState)
    IMPLEMENT_PROPERTY(LightingDebugComponentState, DrawLightLocators)
    IMPLEMENT_PROPERTY(LightingDebugComponentState, DrawLightInfluences)
END_IMPLEMENT_PROPERTIES(LightingDebugComponentState)

LightingDebugComponentState::LightingDebugComponentState()
{
    BEGIN_INIT_PROPERTIES(LightingDebugComponentState)
        INIT_PROPERTY(DrawLightLocators)
        INIT_PROPERTY(DrawLightInfluences)
    END_INIT_PROPERTIES()
}

Wnd::WindowComponentState* LightingDebugComponent::CreateState(const Wnd::ComponentContext& context)
{
    return PG_NEW(mAlloc, -1, "LightingDebugComponentState", Pegasus::Alloc::PG_MEM_PERM) LightingDebugComponentState();
}

void LightingDebugComponent::DestroyState(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
    PG_DELETE(mAlloc, state);
}

void LightingDebugComponent::Load(Core::IApplicationContext* appContext)
{
    //locator program
    mLocatorProgram = appContext->GetShaderManager()->CreateProgram();
    {
        const char locatorVsShader[] =
            "#include \"RenderSystems/Lighting/LightingCore.h\"\n"
            "StructuredBuffer<LightInfo> LightInputBuffer;\n"
            "cbuffer locatorConstants { float4x4 gBillboard; float4x4 gViewProj; float4 eye;}"
            "void main(in float4 p0 : POSITION0, in uint instanceId : SV_InstanceID, out float4 outPos : SV_Position)\n"
            "{\n"
            "   LightInfo info = LightInputBuffer[instanceId];\n"
            "   float3 offset = info.attr1.xyz;\n"
            "   float scale = distance(eye.xyz, offset);\n"
            "   float4 billboarded = mul(float4(p0.xyz*scale,1.0),gBillboard) + float4(offset,0.0);\n"            
            "   outPos = mul(billboarded,gViewProj);\n"
            "}\n";

        const char locatorPsShader[] =
            "float4 main() : SV_Target\n"
            "{\n"
            "   return float4(0.1,1.0,0.1,1.0);\n"
            "}\n";

        Shader::ShaderStageRef vs = appContext->GetShaderManager()->CreateShader();
        vs->SetSource(Pegasus::Shader::VERTEX, locatorVsShader, sizeof(locatorVsShader));

        Shader::ShaderStageRef ps = appContext->GetShaderManager()->CreateShader();
        ps->SetSource(Pegasus::Shader::FRAGMENT, locatorPsShader, sizeof(locatorPsShader));

        mLocatorProgram->SetShaderStage(vs);
        mLocatorProgram->SetShaderStage(ps);

        Render::GetUniformLocation(mLocatorProgram, "locatorConstants", mLocatorConstantUniform);
        mLocatorConstantBuffer = Render::CreateUniformBuffer(sizeof(LocatorConstants));

        Render::GetUniformLocation(mLocatorProgram, "LightInputBuffer", mLightBufferUniform);
    }

    mSphereLightProgram = appContext->GetShaderManager()->CreateProgram();
    {
        const char sphereVsShader[] =
            "#include \"RenderSystems/Lighting/LightingCore.h\"\n"
            "#include \"RenderSystems/Camera/Common.h\"\n"
            "StructuredBuffer<LightInfo> LightInputBuffer;\n"
            "void main(in float4 p0 : POSITION0, in uint instanceId : SV_InstanceID, out float4 outPos : SV_Position)\n"
            "{\n"
            "   uint lightId = instanceId / 2;"
            "   bool isInnerSphere = (instanceId % 2) == 0;"
            "   LightInfo info = LightInputBuffer[lightId];\n"
            "   if (info.attr3.x != LIGHTTYPE_SPHERE) { outPos = float4(2.0,2.0,2.0,1.0); return;}\n"
            "   float4 offsetScale = info.attr1;\n"
            "   float  innerSphereScale = info.attr2.x;\n"
            "   float3 scaledPos = p0.xyz*(isInnerSphere ? innerSphereScale : offsetScale.w);\n"
            "   outPos = mul(float4(scaledPos + offsetScale.xyz,1.0),gViewProj);\n"
            "}\n";

        const char spherePsShader[] =
            "float4 main() : SV_Target\n"
            "{\n"
            "   return float4(0.3,1.0,0.1,1.0);\n"
            "}\n";

        Shader::ShaderStageRef vs = appContext->GetShaderManager()->CreateShader();
        vs->SetSource(Pegasus::Shader::VERTEX, sphereVsShader, sizeof(sphereVsShader));

        Shader::ShaderStageRef ps = appContext->GetShaderManager()->CreateShader();
        ps->SetSource(Pegasus::Shader::FRAGMENT, spherePsShader, sizeof(spherePsShader));

        mSphereLightProgram->SetShaderStage(vs);
        mSphereLightProgram->SetShaderStage(ps);

        Render::GetUniformLocation(mSphereLightProgram, "LightInputBuffer", mSphereProgramLightBufferUniform);
    }

    mSpotLightProgram = appContext->GetShaderManager()->CreateProgram();
    {
        const char spotVsShader[] =
            "#include \"RenderSystems/Lighting/LightingCore.h\"\n"
            "#include \"RenderSystems/Camera/Common.h\"\n"
            "StructuredBuffer<LightInfo> LightInputBuffer;\n"
            "void main(\n"
            "in float4 p0 : POSITION0,\n"
            "in uint vertexId   : SV_VertexID,\n"
            "in uint instanceId : SV_InstanceID,\n"
            " out float4 outPos : SV_Position)\n"
            "{\n"
            "   LightInfo info = LightInputBuffer[instanceId];\n"
            "   float4 offsetScale = info.attr1;\n"
            "   float4 dirAndSpeed = info.attr2;\n"
            "   float3 scaledPos = p0.xyz*offsetScale.w;\n"
            "   if (info.attr3.x != LIGHTTYPE_SPOT) { outPos = float4(2.0,2.0,2.0,1.0); return;}\n"
            "   if (vertexId > 0)\n"
            "   {\n"
            "       p0.xy *= dirAndSpeed.w;\n"
            "       p0.z  = offsetScale.w;\n"
            "       float3 zAxis = normalize(dirAndSpeed.xyz);\n"
            "       float3 yAxis = abs(dot(zAxis,float3(0.0,1.0,0.0))) > 0.999 ? float3(0.0,0.0,-1.0) : float3(0.0,1.0,0.0);\n"
            "       float3 xAxis = normalize(cross(yAxis,zAxis));\n"
            "       yAxis = cross(zAxis,xAxis);\n"
            "       float3x3 dirMatrix = float3x3(xAxis,yAxis,zAxis);\n"
            "       p0.xyz = mul(p0.xyz,dirMatrix);\n"
            "   }\n"
            "   p0.xyz += offsetScale.xyz;\n"
            "   outPos = mul(p0, gViewProj);\n"
            "}\n";

        const char spotPsShader[] =
            "float4 main() : SV_Target\n"
            "{\n"
            "   return float4(0.3,1.0,0.1,1.0);\n"
            "}\n";

        Shader::ShaderStageRef vs = appContext->GetShaderManager()->CreateShader();
        vs->SetSource(Pegasus::Shader::VERTEX, spotVsShader, sizeof(spotVsShader));

        Shader::ShaderStageRef ps = appContext->GetShaderManager()->CreateShader();
        ps->SetSource(Pegasus::Shader::FRAGMENT, spotPsShader, sizeof(spotPsShader));

        mSpotLightProgram->SetShaderStage(vs);
        mSpotLightProgram->SetShaderStage(ps);

        Render::GetUniformLocation(mSpotLightProgram, "LightInputBuffer", mSpotLightProgramBufferUniform);
    }

    //this is the mesh config used by all procedurally generated debug lines.
    MeshConfiguration meshConfig;
    meshConfig.SetIsIndexed(true);
    meshConfig.SetIsDynamic(false);
    meshConfig.SetMeshPrimitiveType(MeshConfiguration::LINE);
    MeshInputLayout il;
    il.GenerateEditorLayout(MeshInputLayout::USE_POSITION);
    meshConfig.SetInputLayout(il);

    MeshManager* mm = appContext->GetMeshManager();
    mLocatorMesh = mm->CreateMeshNode();
    {
        //procedurally create a little light bulb mesh.
        MeshGeneratorRef generator = mm->CreateMeshGeneratorNode("CustomGenerator");
        CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*generator));
        customGenerator->SetConfiguration(meshConfig);
        MeshDataRef meshData = customGenerator->EditMeshData();

        //generate vertices
        const float radii = 0.04f;
        const float angleIncrement = Math::P_2_PI / 8.0f;
        short bulbVertexCount = 8;
        Vec4 leftVertex;
        Vec4 rightVertex;
        for (int i = 0; i < bulbVertexCount; ++i)
        {
            float fi = static_cast<float>(i);
            float angle = angleIncrement * fi;
            Vec4 vert = Vec4(radii*Vec3(Math::Sin(angle),-Math::Cos(angle),0.0f),1.0f);
            meshData->PushVertex(vert,0);
            if (i > 0 && i < bulbVertexCount-1)
            {
                meshData->PushIndex(i);
                meshData->PushIndex(i+1);
            }

            if (i == 1)
            {
                leftVertex = vert;
            }
            else if (i == bulbVertexCount - 1)
            {
                rightVertex = vert;
            }
        }
        short firstIndex = 1;
        short lastIndex = bulbVertexCount - 1;
        float bulbBottomOffset = 0.8f*radii;
        meshData->PushVertex(Vec4(leftVertex.x,leftVertex.y-bulbBottomOffset,leftVertex.z,1.0), 0);
        meshData->PushVertex(Vec4(rightVertex.x,rightVertex.y-bulbBottomOffset,rightVertex.z,1.0), 0);

        meshData->PushIndex(firstIndex);
        meshData->PushIndex(lastIndex+1);

        meshData->PushIndex(lastIndex);
        meshData->PushIndex(lastIndex+2);
        meshData->PushIndex(lastIndex + 1);
        meshData->PushIndex(lastIndex + 2);

        mLocatorMesh->SetGeneratorInput(generator);
    }


    mSphereLightMesh = mm->CreateMeshNode();
    {
        //procedurally create a little light bulb mesh.
        MeshGeneratorRef generator = mm->CreateMeshGeneratorNode("CustomGenerator");
        CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*generator));
        customGenerator->SetConfiguration(meshConfig);
        MeshDataRef meshData = customGenerator->EditMeshData();
        const int arcVertCount = 32;
        const float arcVertCountF = (float)arcVertCount;
        const float arcPartAngle = Math::P_2_PI / arcVertCountF;
        short arcVertIndex = 0;
        for (int axis = 0; axis < 3; ++axis)
        {
            for (int i = 0; i < arcVertCount; ++i)
            {
                float angle = arcPartAngle * (float)i;
                float s = Math::Sin(angle);
                float c = Math::Cos(angle);
                Vec4 v;
                if (axis == 0)
                {
                    v = Vec4(s,0.0f,c,1.0f);
                }
                else if (axis == 1)
                {
                    v = Vec4(s,c,0.0f,1.0f);
                }
                else
                {
                    v = Vec4(0.0f,s,c,1.0f);
                }
                meshData->PushVertex(v,0);
                if (i < arcVertCount - 1)
                {
                    meshData->PushIndex(arcVertIndex);
                    meshData->PushIndex(arcVertIndex + 1);
                }
                else
                {
                    meshData->PushIndex(arcVertIndex);
                    meshData->PushIndex(arcVertIndex-i);
                }
                arcVertIndex++;
            }
        }
        mSphereLightMesh->SetGeneratorInput(generator);
    }


    mSpotLightMesh = mm->CreateMeshNode();
    {
        //procedurally create a little light bulb mesh.
        MeshGeneratorRef generator = mm->CreateMeshGeneratorNode("CustomGenerator");
        CustomGenerator* customGenerator = static_cast<CustomGenerator*>(&(*generator));
        customGenerator->SetConfiguration(meshConfig);
        MeshDataRef meshData = customGenerator->EditMeshData();
        const int arcVertCount = 14;
        const float arcVertCountF = (float)arcVertCount;
        const float vertAngle = Math::P_2_PI / arcVertCountF;
        meshData->PushVertex(Vec4(0.0f,0.0f,0.0f,1.0f),0);
        for (int i = 0 ; i < arcVertCount; ++i)
        {
            float angle = (float)i * vertAngle;
            float s = Math::Sin(angle);
            float c = Math::Cos(angle);
            meshData->PushVertex(Vec4(s,c,1.0f,1.0f), 0);
            meshData->PushIndex((short)i+1);
            meshData->PushIndex((i < arcVertCount - 1) ? (short)i+2 : 1);
        }
        for (int i = 0; i < arcVertCount; ++i)
        {
            meshData->PushIndex(0);
            meshData->PushIndex((short)i+1);
        }
        mSpotLightMesh->SetGeneratorInput(generator);
    }

    //default raster / blend states
    RasterizerConfig rasterConfig;
    rasterConfig.mCullMode = RasterizerConfig::NONE_CM;
    rasterConfig.mDepthFunc = RasterizerConfig::NONE_DF;
    mRasterState =  CreateRasterizerState(rasterConfig);

    BlendingConfig blendConfig;
    blendConfig.mBlendingOperator = BlendingConfig::NONE_BO;
    mBlendState = CreateBlendingState(blendConfig);
}

void LightingDebugComponent::Update(Core::IApplicationContext* appContext)
{
}

void LightingDebugComponent::WindowUpdate(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{
}

void LightingDebugComponent::Render(const Wnd::ComponentContext& context, Wnd::WindowComponentState* state)
{    
    LightingDebugComponentState* lightDebugState = static_cast<LightingDebugComponentState*>(state);
    if (lightDebugState->GetDrawLightLocators())
    {
        Render::BeginMarker("LightDebugComponent");
        DispatchDefaultRenderTarget();
        int lightCount = gLightingSystemInstance->GetActiveLightCount();
        if (lightCount > 0)
        {
            Render::BeginMarker("DrawDebugLocators");
            SetRasterizerState(mRasterState);
            SetBlendingState(mBlendState);

            //setup state
            LocatorConstants lc;

            CameraRef dispatchedCam = gCameraSystem->GetCurrentCamera();
            const Camera::Camera::GpuCamData& camGpuData = dispatchedCam->GetGpuData();
            float bbScale = dispatchedCam->GetFov()*0.15f;
            Mat44 scale = MAT44_IDENTITY*bbScale;       
            Mat44 billboard = camGpuData.invView;
            billboard.m14 = 0.0f;
            billboard.m24 = 0.0f;
            billboard.m34 = 0.0f;    
            Mult44_44(lc.billboard,billboard,scale);
            lc.viewProj = camGpuData.viewProj;
            lc.eyeX = camGpuData.invView.m14;
            lc.eyeY = camGpuData.invView.m24;
            lc.eyeZ = camGpuData.invView.m34;
            lc.eyeW = 1.0f;
            SetBuffer(mLocatorConstantBuffer, &lc);

            SetProgram(mLocatorProgram);
            SetUniformBuffer(mLocatorConstantUniform, mLocatorConstantBuffer);
            SetUniformBufferResource(mLightBufferUniform, gLightingSystemInstance->GetCulledLightBuffer());

            SetMesh(mLocatorMesh);
            DrawInstanced(lightCount);
            Render::EndMarker();
            if (lightDebugState->GetDrawLightInfluences())
            {
                Render::BeginMarker("DrawDebugInfluences");
                SetProgram(mSphereLightProgram);
                SetUniformBufferResource(mSphereProgramLightBufferUniform, gLightingSystemInstance->GetCulledLightBuffer());
                SetMesh(mSphereLightMesh);
                DrawInstanced(lightCount*2);//even instances are inner sphere, odd instances are outer sphere

                SetProgram(mSpotLightProgram);
                SetUniformBufferResource(mSpotLightProgramBufferUniform, gLightingSystemInstance->GetCulledLightBuffer());
                SetMesh(mSpotLightMesh);
                DrawInstanced(lightCount);
                Render::EndMarker();
            }
            Render::EndMarker();
        }
    }
}

void LightingDebugComponent::Unload(Core::IApplicationContext* appContext)
{
}

#endif
#else
    PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
