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
#include "Pegasus/Math/Quaternion.h"


//Constructor
GeometryTestBlock::GeometryTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
    : Pegasus::Timeline::Block(allocator, appContext)

{

}

//Destructor
GeometryTestBlock::~GeometryTestBlock()
{

}

//! Initializer
void GeometryTestBlock::Initialize()
{
    // setup shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mBlockProgram = shaderManager->CreateProgram("CubeProgram");
    
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();
    fileLoadProperties.mPath = "Shaders\\Cube.vs";
    mBlockProgram->SetShaderStage(  
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );

    fileLoadProperties.mPath = "Shaders\\Cube.ps";
    mBlockProgram->SetShaderStage(  
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );

    mDiscoSpeaker = shaderManager->CreateProgram("DiscoSpeaker");
    
    fileLoadProperties.mPath = "Shaders\\DiscoSpeaker.vs";
    mDiscoSpeaker->SetShaderStage(
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );
    fileLoadProperties.mPath = "Shaders\\DiscoSpeaker.ps";
    mDiscoSpeaker->SetShaderStage(
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );

    bool updated = false; 
    mBlockProgram->GetUpdatedData(updated);
    updated = false;
    mDiscoSpeaker->GetUpdatedData(updated);

    // setup uniforms
    Pegasus::Render::GetUniformLocation(mBlockProgram, "cubeTex", mCubeTextureUniform);
    Pegasus::Render::GetUniformLocation(mBlockProgram, "uniformState", mUniformBlock);
    Pegasus::Render::CreateUniformBuffer(sizeof(mState), mUniformStateBuffer);
    Pegasus::Math::SetIdentity(mState.mRotation);

    Pegasus::Render::GetUniformLocation(mDiscoSpeaker, "aspect",   mAspectUniform);
    Pegasus::Render::GetUniformLocation(mDiscoSpeaker, "time",   mTimeUniform);

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

    //setup render targets
    Pegasus::Render::RenderTargetConfig c;
    c.mWidth = 512;
    c.mHeight = 512;
    Pegasus::Render::CreateRenderTarget(c, mCubeFaceRenderTarget);
}

//! Shutdown used by block
void GeometryTestBlock::Shutdown()
{
    Pegasus::Render::DeleteBuffer(mUniformStateBuffer);
    Pegasus::Render::DeleteRenderTarget(mCubeFaceRenderTarget);
}

//! Render function
void GeometryTestBlock::Render(float beat, Pegasus::Wnd::Window * window)
{

    //figure out aspect ratio
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);
    float aspect = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

    // rendering to the face of the disco a normal map and a detail map
    Pegasus::Render::Dispatch(mCubeFaceRenderTarget); //use the fractal surface, no depth
    Pegasus::Render::Clear(/*color*/true, /*depth*/false, /*stencil*/false); //clear the surface
    Pegasus::Render::Dispatch(mDiscoSpeaker); // dispatch the shader
    Pegasus::Render::Dispatch(mQuad); // screen space

    Pegasus::Render::SetUniform(mAspectUniform, aspect);
    Pegasus::Render::SetUniform(mTimeUniform, beat);

    Pegasus::Render::Draw();

    // render the cube with the speaker
    Pegasus::Render::DispatchDefaultRenderTarget(Pegasus::Render::Viewport(viewportWidth, viewportHeight));
    Pegasus::Render::Dispatch(mBlockProgram);

    Pegasus::Math::Vec3 rotAxis(0.3f, 0.4f, 0.01f);
    Pegasus::Math::SetRotation(mState.mRotation, rotAxis, (float)beat * 0.92f);

    // do aspect ratio correctness on y axis
    mState.mRotation.m21 *= aspect;
    mState.mRotation.m22 *= aspect;
    mState.mRotation.m23 *= aspect;

    Pegasus::Render::SetBuffer(mUniformStateBuffer, &mState, sizeof(mState));
    Pegasus::Render::SetUniform(mUniformBlock, mUniformStateBuffer);
    Pegasus::Render::SetUniform(mCubeTextureUniform, mCubeFaceRenderTarget);

    // todo - add functions to handle depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_GREATER);

    Pegasus::Render::Dispatch(mCubeMesh);
    Pegasus::Render::Draw();
    
    glDisable(GL_DEPTH_TEST);
}
