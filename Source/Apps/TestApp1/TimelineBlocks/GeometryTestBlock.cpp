/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeometryTestBlock.cpp
//! \author	Kleber Garcia
//! \date	16th June 2014
//! \brief	Geometry test

#include "Apps/TestApp1/TimelineBlocks/GeometryTestBlock.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "Pegasus/Mesh/MeshManager.h"

#include "Pegasus/Math/Quaternion.h"
#include "Pegasus/Mesh/Generator/IcosphereGenerator.h"
#include "Pegasus/Shader/ShaderManager.h"

#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"

GeometryTestBlock::GeometryTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
    : Pegasus::Timeline::Block(allocator, appContext)

{

}

//----------------------------------------------------------------------------------------

GeometryTestBlock::~GeometryTestBlock()
{

}

void geomTestFun_callback(Pegasus::BlockScript::FunCallbackContext& context)
{
    Pegasus::BlockScript::FunParamStream stream(context);
    stream.SubmitReturn<int>(20);
}

//----------------------------------------------------------------------------------------

void GeometryTestBlock::Initialize()
{

    if (HasScript())
    {
        Pegasus::Timeline::TimelineScriptRef timelineScript = GetScript();
        Pegasus::BlockScript::BlockScript* bs = timelineScript->GetBlockScript();
        Pegasus::BlockScript::BlockLib* newLib = GetBlockScriptManager()->CreateBlockLib();
        Pegasus::BlockScript::FunctionDeclarationDesc funDesc = {
            "geomTestFun",
            "int",
            { nullptr },
            { nullptr },
            geomTestFun_callback
        };
        newLib->CreateIntrinsicFunctions(&funDesc, 1);
        bs->IncludeLib(newLib);
    }
    //setup raster states
    Pegasus::Render::RasterizerConfig rasterConfig;
    rasterConfig.mCullMode = Pegasus::Render::RasterizerConfig::CW_CM;
    rasterConfig.mDepthFunc = Pegasus::Render::RasterizerConfig::GREATER_DF;

    Pegasus::Render::CreateRasterizerState(rasterConfig, mCurrentBlockRasterState);
    rasterConfig.mCullMode = Pegasus::Render::RasterizerConfig::NONE_CM;
    rasterConfig.mDepthFunc = Pegasus::Render::RasterizerConfig::NONE_DF;

    Pegasus::Render::CreateRasterizerState(rasterConfig, mDefaultRasterState);
    
    // setup shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mBlockProgram = shaderManager->LoadProgram("Programs/CubeProgram.pas");
    mDiscoSpeaker = shaderManager->LoadProgram("Programs/discospeaker.pas");
    mBlurHorizontal = shaderManager->LoadProgram("Programs/blurhorizontal.pas");
    mBlurVertical = shaderManager->LoadProgram("Programs/blurvertical.pas");
    mComposite = shaderManager->LoadProgram("Programs/composite.pas");

    bool updated = false; 
    mBlockProgram->GetUpdatedData(updated);
    updated = false;
    mDiscoSpeaker->GetUpdatedData(updated);

    // setup uniforms
    Pegasus::Render::GetUniformLocation(mBlockProgram, "cubeTex", mCubeTextureUniform);
    Pegasus::Render::GetUniformLocation(mBlockProgram, "uniformState", mUniformBlock);
    Pegasus::Render::CreateUniformBuffer(sizeof(mState), mUniformStateBuffer);
    Pegasus::Math::SetIdentity(mState.mRotation);

    Pegasus::Render::GetUniformLocation(mDiscoSpeaker, "uniformState",   mSpeakerUniformBlock);
    Pegasus::Render::CreateUniformBuffer(sizeof(mSpeakerState), mSpeakerStateBuffer);

    Pegasus::Render::GetUniformLocation(mComposite, "inputTexture1", mCompositeInput1);
    Pegasus::Render::GetUniformLocation(mComposite, "inputTexture2", mCompositeInput2);
    Pegasus::Render::GetUniformLocation(mBlurHorizontal, "inputTexture", mHorizontalInput);
    Pegasus::Render::GetUniformLocation(mBlurVertical, "inputTexture", mVerticalInput);

    //setup meshes
    Pegasus::Mesh::MeshManager * const meshManager = GetMeshManager();
    mCubeMesh = meshManager->CreateMeshNode();
    mCubeMesh->SetGeneratorInput(
        meshManager->CreateMeshGeneratorNode("BoxGenerator")
    );

    mQuad = meshManager->CreateMeshNode();
    mQuad->SetGeneratorInput(
        meshManager->CreateMeshGeneratorNode("QuadGenerator")
    );
    int Degrees[MAX_SPHERES] = {3,2,1,1};
    for (int i = 0; i < MAX_SPHERES; ++i)
    {
        mSphereMeshes[i] = meshManager->CreateMeshNode();
        Pegasus::Mesh::MeshGeneratorRef generator = meshManager->CreateMeshGeneratorNode("IcosphereGenerator");
        Pegasus::Mesh::IcosphereGenerator * icosphere = static_cast<Pegasus::Mesh::IcosphereGenerator*>(generator);
        icosphere->SetDegree(Degrees[i]);
        icosphere->SetRadius( 0.45f);
        mSphereMeshes[i]->SetGeneratorInput(
            generator
        );
        //force update
        mSphereMeshes[i]->GetUpdatedMeshData();
    }

    //setup render targets
    Pegasus::Render::RenderTargetConfig c;
    c.mWidth = 512;
    c.mHeight = 512;
    Pegasus::Render::CreateRenderTarget(c, mCubeFaceRenderTarget);

    c.mWidth = 1280;
    c.mHeight = 720;
    Pegasus::Render::CreateRenderTarget(c, mTempTarget1);
    c.mWidth /= 4;
    c.mHeight /= 4;
    Pegasus::Render::CreateRenderTarget(c, mTempTarget2);
    Pegasus::Render::CreateRenderTarget(c, mTempTarget3);

    Pegasus::Render::CubeMapConfig cubeDesc;
    cubeDesc.mWidth = 256;
    cubeDesc.mHeight = 256;
    
    Pegasus::Render::CreateCubeMap(cubeDesc, mCubeMap);
}

//----------------------------------------------------------------------------------------

void GeometryTestBlock::Shutdown()
{
    Pegasus::Render::DeleteBuffer(mUniformStateBuffer);
    Pegasus::Render::DeleteBuffer(mSpeakerStateBuffer);
    Pegasus::Render::DeleteRenderTarget(mTempTarget1);
    Pegasus::Render::DeleteRenderTarget(mTempTarget2);
    Pegasus::Render::DeleteRenderTarget(mTempTarget3);
    Pegasus::Render::DeleteRenderTarget(mCubeFaceRenderTarget);
    Pegasus::Render::DeleteRasterizerState(mCurrentBlockRasterState);
    Pegasus::Render::DeleteRasterizerState(mDefaultRasterState);
    Pegasus::Render::DeleteCubeMap(mCubeMap);
}

//----------------------------------------------------------------------------------------

void GeometryTestBlock::Update(float beat, Pegasus::Wnd::Window * window)
{
    UpdateViaScript(beat, window);
}

//----------------------------------------------------------------------------------------

void GeometryTestBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    RenderViaScript(beat, window);
    //figure out aspect ratio
    const unsigned int viewportWidth = window->GetWidth();
    const unsigned int viewportHeight = window->GetHeight();
    const float aspect = window->GetRatio();

    ///////////////////////////////////////////////
    //******** PASS 1: texture of box ***********//
    ///////////////////////////////////////////////
    // rendering to the face of the disco a normal map and a detail map
    Pegasus::Render::SetViewport(mCubeFaceRenderTarget);
    Pegasus::Render::SetRenderTarget(mCubeFaceRenderTarget); //use the fractal surface, no depth
    Pegasus::Render::Clear(/*color*/true, /*depth*/false, /*stencil*/false); //clear the surface
    Pegasus::Render::SetProgram(mDiscoSpeaker); // dispatch the shader
    Pegasus::Render::SetMesh(mQuad); // screen space

    mSpeakerState.time = beat;
    Pegasus::Render::SetBuffer(mSpeakerStateBuffer, &mSpeakerState);
    Pegasus::Render::SetUniformBuffer(mSpeakerUniformBlock, mSpeakerStateBuffer);

    Pegasus::Render::Draw();

    ///////////////////////////////////////////////
    //******** PASS 2: scene          ***********//
    ///////////////////////////////////////////////
    Pegasus::Render::SetViewport(mTempTarget1);
    Pegasus::Render::SetRenderTarget(mTempTarget1);
    Pegasus::Render::Clear(/*color*/true, /*depth*/false, /*stencil*/false); //clear the surface
    Pegasus::Render::SetProgram(mBlockProgram);

    Pegasus::Math::Vec3 rotAxis(0.3f, 0.4f, 0.01f);
    Pegasus::Math::SetRotation(mState.mRotation, rotAxis, (float)beat * 0.92f);

    // do aspect ratio correctness on y axis
    mState.mRotation.m21 *= aspect;
    mState.mRotation.m22 *= aspect;
    mState.mRotation.m23 *= aspect;

    Pegasus::Render::SetBuffer(mUniformStateBuffer, &mState, sizeof(mState));
    Pegasus::Render::SetUniformBuffer(mUniformBlock, mUniformStateBuffer);
    Pegasus::Render::SetUniformTextureRenderTarget(mCubeTextureUniform, mCubeFaceRenderTarget);

    Pegasus::Render::SetRasterizerState(mCurrentBlockRasterState);

    Pegasus::Render::SetMesh(mCubeMesh);
    Pegasus::Render::Draw();

    for (int i = 0; i < MAX_SPHERES; ++i)
    {
        Pegasus::Render::SetMesh(mSphereMeshes[i]);

        float fi = 2.0f * ((float)i / (float)MAX_SPHERES) - 1.0f;
        
        static const int maxDepth = 4;
        for (int j = 0; j < maxDepth; ++j)
        {
            float fj = 2.0f * ((float)j / (float)maxDepth) - 1.0f; 
            float ox = 2.7f*Pegasus::Math::Cos(beat * 0.6f + 4.7f*fj * fi);
            float oy = 2.7f*Pegasus::Math::Sin(beat * 0.6f - 6.0f*fj + 3.0f*fi);
            mState.mRotation.m14 = 6.0f * fi + ox;
            mState.mRotation.m24 = 6.0f * fj + oy;
            mState.mRotation.m34 = 0.5;
            mState.mRotation.m44 = 1.0f;
            Pegasus::Render::SetBuffer(mUniformStateBuffer, &mState, sizeof(mState));
            Pegasus::Render::Draw();
        }
    }
    
    ///////////////////////////////////////////////
    //******** PASS 3: horizontal blur***********//
    ///////////////////////////////////////////////
    Pegasus::Render::SetViewport(mTempTarget2);
    Pegasus::Render::SetRenderTarget(mTempTarget2);
    Pegasus::Render::SetProgram(mBlurHorizontal);
    Pegasus::Render::SetMesh(mQuad);
    Pegasus::Render::SetUniformTextureRenderTarget(mHorizontalInput, mTempTarget1);
    Pegasus::Render::Draw();
    
    ///////////////////////////////////////////////
    //******** PASS 4: vertical   blur***********//
    ///////////////////////////////////////////////
    Pegasus::Render::SetRenderTarget(mTempTarget3);
    Pegasus::Render::SetProgram(mBlurVertical);
    Pegasus::Render::SetMesh(mQuad);
    Pegasus::Render::SetUniformTextureRenderTarget(mVerticalInput, mTempTarget2);
    Pegasus::Render::Draw();

    ///////////////////////////////////////////////
    //******** PASS 5: final compositing ********//
    ///////////////////////////////////////////////
    Pegasus::Render::SetViewport(Pegasus::Render::Viewport(viewportWidth, viewportHeight));
    Pegasus::Render::DispatchDefaultRenderTarget();
    Pegasus::Render::SetProgram(mComposite);
    Pegasus::Render::SetMesh(mQuad);
    Pegasus::Render::SetUniformTextureRenderTarget(mCompositeInput1, mTempTarget1); //original scene
    Pegasus::Render::SetUniformTextureRenderTarget(mCompositeInput2, mTempTarget3); //bloomed scene
    Pegasus::Render::Draw();

    Pegasus::Render::SetRasterizerState(mDefaultRasterState);

}
