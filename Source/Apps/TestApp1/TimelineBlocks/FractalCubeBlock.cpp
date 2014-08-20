/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FractalCubeBlock.cpp
//! \author	Kevin Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the FractalCube effect (grayscale fractal with no shadow)

#include "TimelineBlocks/FractalCubeBlock.h"
#include "Pegasus/Render/Render.h"

#if PEGASUS_GAPI_GL

static const char * VERTEX_SHADER = "Shaders\\glsl\\Cubefractal.vs";
static const char * FRAGMENT_SHADER = "Shaders\\glsl\\CubeFractal.ps";

#elif PEGASUS_GAPI_DX

static const char * VERTEX_SHADER = "Shaders\\hlsl\\Cubefractal.vs";
static const char * FRAGMENT_SHADER = "Shaders\\hlsl\\CubeFractal.ps";

#endif

//----------------------------------------------------------------------------------------

FractalCubeBlock::FractalCubeBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
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
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->CreateProgram("FractalCube");
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();

    fileLoadProperties.mPath = VERTEX_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    fileLoadProperties.mPath = FRAGMENT_SHADER;
    mProgram->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

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

void FractalCubeBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mQuad->Update();

    Pegasus::Render::Dispatch(mProgram);
    Pegasus::Render::Dispatch(mQuad);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    // Set up uniforms
    mState.ratio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    mState.time = currentTime;
    Pegasus::Render::SetBuffer(mStateBuffer, &mState);
    Pegasus::Render::SetUniform(mStateBufferUniform, mStateBuffer);
    
    Pegasus::Render::Draw();
}
