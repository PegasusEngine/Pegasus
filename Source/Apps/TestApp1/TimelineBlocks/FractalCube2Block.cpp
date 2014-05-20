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
#include "Pegasus/Render/Render.h"

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
    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);
    mQuad->GetUpdatedMeshData();

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
    mScreenRatioUniform = 0;//glGetUniformLocation(mProgramData->GetHandle(), "screenRatio");
    mTimeUniform = 1;//glGetUniformLocation(mProgramData->GetHandle(), "time");
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
}

//----------------------------------------------------------------------------------------

void FractalCube2Block::Render(float beat, Pegasus::Wnd::Window * window)
{

    Pegasus::Render::Dispatch(mProgram);
    Pegasus::Render::Dispatch(mQuad);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendEquation(GL_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glUniform1f(mTimeUniform, currentTime);
    glUniform1f(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
    
    Pegasus::Render::Draw();

    glDisable(GL_BLEND);
}
