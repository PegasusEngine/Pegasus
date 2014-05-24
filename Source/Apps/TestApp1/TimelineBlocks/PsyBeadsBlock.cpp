/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PsyBeadsBlock.cpp
//! \author	Kevin Boulanger
//! \date	10th January 2014
//! \brief	Timeline block for the PsyBeads effect (vertical columns with regular beads)

#include "TimelineBlocks/PsyBeadsBlock.h"
#include "Pegasus/Render/Render.h"

static const char * VERTEX_SHADER = "Shaders\\Psybeads.vs";
static const char * FRAGMENT_SHADER = "Shaders\\Psybeads.ps";

//----------------------------------------------------------------------------------------
    
PsyBeadsBlock::PsyBeadsBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
:   Pegasus::Timeline::Block(allocator, appContext)
{
}

//----------------------------------------------------------------------------------------

PsyBeadsBlock::~PsyBeadsBlock()
{
}

//----------------------------------------------------------------------------------------

void PsyBeadsBlock::Initialize()
{
    //Set up quad
    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuad = GetMeshManager()->CreateMeshNode();
    mQuad->SetGeneratorInput(quadGenerator);
    mQuad->GetUpdatedMeshData();

    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->CreateProgram("PsyBeads");
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
    Pegasus::Render::GetUniformLocation(mProgram, "screenRatio", mScreenRatioUniform);
    Pegasus::Render::GetUniformLocation(mProgram, "time", mTimeUniform);
}

//----------------------------------------------------------------------------------------

void PsyBeadsBlock::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
}

//----------------------------------------------------------------------------------------

void PsyBeadsBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::Dispatch(mProgram);
    Pegasus::Render::Dispatch(mQuad);

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    Pegasus::Render::SetUniform(mTimeUniform, currentTime);
    Pegasus::Render::SetUniform(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));

    Pegasus::Render::Draw();
}
