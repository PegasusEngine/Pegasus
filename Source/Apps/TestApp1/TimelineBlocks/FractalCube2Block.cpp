/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCube2Block.cpp
//! \author	Kevin Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube2 effect (colored fractal with shadows)

#include "TimelineBlocks/FractalCube2Block.h"

#if PEGASUS_GAPI_GL

static const char * VERTEX_SHADER = "Shaders\\glsl\\Cubefractal2.vs";
static const char * FRAGMENT_SHADER = "Shaders\\glsl\\CubeFractal2.ps";

#elif PEGASUS_GAPI_DX

static const char * VERTEX_SHADER = "Shaders\\hlsl\\Cubefractal2.vs";
static const char * FRAGMENT_SHADER = "Shaders\\hlsl\\CubeFractal2.ps";

#endif

//----------------------------------------------------------------------------------------

FractalCube2Block::FractalCube2Block(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
:   Pegasus::Timeline::Block(allocator, appContext)
{
}

//----------------------------------------------------------------------------------------

FractalCube2Block::~FractalCube2Block()
{
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Initialize()
{
    //Setup blending states
    Pegasus::Render::BlendingConfig blendingConfig;
    blendingConfig.mBlendingOperator = Pegasus::Render::BlendingConfig::ADD_BO;
    blendingConfig.mSource = Pegasus::Render::BlendingConfig::ONE_M;
    blendingConfig.mDest = Pegasus::Render::BlendingConfig::ONE_M;
    Pegasus::Render::CreateBlendingState(blendingConfig, mCurrentBlockBlendingState);

    blendingConfig.mBlendingOperator = Pegasus::Render::BlendingConfig::NONE_BO;
    Pegasus::Render::CreateBlendingState(blendingConfig, mDefaultBlendingState);

    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);

    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->CreateProgram("FractalCube2");
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();

    fileLoadProperties.mPath = VERTEX_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    fileLoadProperties.mPath = FRAGMENT_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    // Set up shader uniforms
    Pegasus::Render::CreateUniformBuffer(sizeof(mState), mStateBuffer);
    Pegasus::Render::GetUniformLocation(mProgram, "uniformState", mStateBufferUniform);
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Shutdown()
{
    Pegasus::Render::DeleteBlendingState(mCurrentBlockBlendingState);
    Pegasus::Render::DeleteBlendingState(mDefaultBlendingState);
    Pegasus::Render::DeleteBuffer(mStateBuffer);
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Update(float beat, Pegasus::Wnd::Window * window)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mQuad->Update();
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::SetProgram(mProgram);
    Pegasus::Render::SetMesh(mQuad);

    // Enable additive blending
    Pegasus::Render::SetBlendingState(mCurrentBlockBlendingState);

    // Set up uniforms
    mState.ratio = window->GetRatio();
    mState.time = beat;
    Pegasus::Render::SetBuffer(mStateBuffer, &mState);
    Pegasus::Render::SetUniformBuffer(mStateBufferUniform, mStateBuffer);
    
    Pegasus::Render::Draw();
    
    Pegasus::Render::SetBlendingState(mDefaultBlendingState);
}
