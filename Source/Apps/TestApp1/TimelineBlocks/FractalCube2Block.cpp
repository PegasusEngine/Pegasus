/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCube2Block.cpp
//! \author	Karolyn Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube2 effect (colored fractal with shadows)

#include "TimelineBlocks/FractalCube2Block.h"

#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Timeline/Timeline.h"

//----------------------------------------------------------------------------------------
#if PEGASUS_ENABLE_RENDER_API

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
    mCurrentBlockBlendingState = Pegasus::Render::CreateBlendingState(blendingConfig);

    blendingConfig.mBlendingOperator = Pegasus::Render::BlendingConfig::NONE_BO;
    mDefaultBlendingState = Pegasus::Render::CreateBlendingState(blendingConfig);

    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);

    // Set up shaders
    mProgram = GetShaderManager()->LoadProgram("Programs/FractalCube2.pas");

    // Set up shader uniforms
    mStateBuffer = Pegasus::Render::CreateUniformBuffer(sizeof(mState));
    Pegasus::Render::GetUniformLocation(mProgram, "uniformState", mStateBufferUniform);
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Shutdown()
{
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Update(const Pegasus::Timeline::UpdateInfo& updateInfo)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mQuad->Update();
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Render(const Pegasus::Timeline::RenderInfo& renderInfo)
{
    Pegasus::Render::SetProgram(mProgram);
    Pegasus::Render::SetMesh(mQuad);

    // Enable additive blending
    Pegasus::Render::SetBlendingState(mCurrentBlockBlendingState);

    // Set up uniforms
    mState.ratio = renderInfo.aspect;
    mState.time = renderInfo.beat;
    Pegasus::Render::SetBuffer(mStateBuffer, &mState);
    Pegasus::Render::SetUniformBuffer(mStateBufferUniform, mStateBuffer);
    
    Pegasus::Render::Draw();
    
    Pegasus::Render::SetBlendingState(mDefaultBlendingState);
}
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif