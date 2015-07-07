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

#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"

//----------------------------------------------------------------------------------------

FractalCube2Block::FractalCube2Block(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
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
    mProgram = GetShaderManager()->LoadProgram("Programs/FractalCube2.pas");

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
