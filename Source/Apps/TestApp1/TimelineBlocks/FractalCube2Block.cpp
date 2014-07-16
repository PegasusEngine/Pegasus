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

static const char * VERTEX_SHADER = "Shaders\\Cubefractal2.vs";
static const char * FRAGMENT_SHADER = "Shaders\\CubeFractal2.ps";

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
    Pegasus::Render::GetUniformLocation(mProgram, "screenRatio", mScreenRatioUniform);
    Pegasus::Render::GetUniformLocation(mProgram, "time", mTimeUniform);
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Shutdown()
{
    Pegasus::Render::DeleteBlendingState(mCurrentBlockBlendingState);
    Pegasus::Render::DeleteBlendingState(mDefaultBlendingState);
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Render(float beat, Pegasus::Wnd::Window * window)
{
    // Update the graph of all textures and meshes, in case they have dynamic data
    mQuad->Update();

    Pegasus::Render::Dispatch(mProgram);
    Pegasus::Render::Dispatch(mQuad);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    // Enable additive blending
    Pegasus::Render::SetBlendingState(mCurrentBlockBlendingState);

    Pegasus::Render::SetUniform(mTimeUniform, currentTime);
    Pegasus::Render::SetUniform(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
    
    Pegasus::Render::Draw();
    
    Pegasus::Render::SetBlendingState(mDefaultBlendingState);
}
