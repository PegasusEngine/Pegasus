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


#if PEGASUS_ENABLE_PROXIES

using namespace Pegasus;
using namespace Pegasus::App;

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
    Pegasus::Render::GetUniformLocation(mTextureRenderProgram, "targetTexture", mTargetTexUniform);

    mQuad = appContext->GetMeshManager()->CreateMeshNode();
    Pegasus::Mesh::MeshGeneratorRef quadGen = appContext->GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad->SetGeneratorInput(quadGen);

    //default raster / blend states
    Pegasus::Render::RasterizerConfig rasterConfig;
    rasterConfig.mCullMode = Pegasus::Render::RasterizerConfig::NONE_CM;
    rasterConfig.mDepthFunc = Pegasus::Render::RasterizerConfig::NONE_DF;
    mRasterState =  Pegasus::Render::CreateRasterizerState(rasterConfig);

    Pegasus::Render::BlendingConfig blendConfig;
    blendConfig.mBlendingOperator = Pegasus::Render::BlendingConfig::NONE_BO;
    mBlendState = Pegasus::Render::CreateBlendingState(blendConfig);
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
        Pegasus::Render::DispatchDefaultRenderTarget();
        Pegasus::Render::Viewport vp(context.mTargetWindow->GetWidth(), context.mTargetWindow->GetHeight());
        Pegasus::Render::SetViewport(vp);
        Pegasus::Render::SetBlendingState(mBlendState);
        Pegasus::Render::SetRasterizerState(mRasterState);
        Pegasus::Render::SetClearColorValue(Pegasus::Math::ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f));
        Pegasus::Render::Clear(true, false, false);
        Pegasus::Render::SetProgram(mTextureRenderProgram);
        Pegasus::Render::SetMesh(mQuad);
        Pegasus::Render::SetUniformTexture(mTargetTexUniform, textureViewState->mTargetTexture);
        Pegasus::Render::Draw();
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

#else
    PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
