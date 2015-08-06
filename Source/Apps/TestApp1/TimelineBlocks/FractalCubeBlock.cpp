/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCubeBlock.cpp
//! \author	Karolyn Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube effect (grayscale fractal with no shadow)

#include "TimelineBlocks/FractalCubeBlock.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Window/Window.h"

//----------------------------------------------------------------------------------------

FractalCubeBlock::FractalCubeBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext* appContext)
:   Pegasus::Timeline::Block(allocator, appContext)
{
}

//----------------------------------------------------------------------------------------

FractalCubeBlock::~FractalCubeBlock()
{
}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Initialize()
{
    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);

    // Set up shaders
    mProgram = GetShaderManager()->LoadProgram("Programs/FractalCube.pas");

    // Force a compilation of the shaders
    bool updated = false;
    mProgram->GetUpdatedData(updated);

    // Set up shader uniforms
    Pegasus::Render::CreateUniformBuffer(sizeof(mState), mStateBuffer);
    Pegasus::Render::GetUniformLocation(mProgram, "uniformState", mStateBufferUniform);

}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
    Pegasus::Render::DeleteBuffer(mStateBuffer);
}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Update(float beat, Pegasus::Wnd::Window * window)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mQuad->Update();
}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::SetProgram(mProgram);
    Pegasus::Render::SetMesh(mQuad);

    // Set up uniforms
    mState.ratio = window->GetRatio();
    mState.time = beat;
    Pegasus::Render::SetBuffer(mStateBuffer, &mState);
    Pegasus::Render::SetUniformBuffer(mStateBufferUniform, mStateBuffer);
    
    Pegasus::Render::Draw();
}
