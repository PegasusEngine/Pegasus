/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	KleberTriangleBlock.cpp
//! \author	Kevin Boulanger
//! \date	11th January 2014
//! \brief	Timeline block for the KleberHomogayTriangle effect

#include "TimelineBlocks/KleberTriangleBlock.h"
#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Render/Render.h"

static const char * VERTEX_SHADER = "Shaders\\Blob.vs";
static const char * FRAGMENT_SHADER = "Shaders\\Blob.ps";

//----------------------------------------------------------------------------------------

KleberTriangleBlock::KleberTriangleBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
:   Pegasus::Timeline::Block(allocator, appContext)
{
}

//----------------------------------------------------------------------------------------

KleberTriangleBlock::~KleberTriangleBlock()
{
}

//----------------------------------------------------------------------------------------

void KleberTriangleBlock::Initialize()
{
    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mProgram = shaderManager->CreateProgram("KleberTriangleBlob");
    mProgram2 = shaderManager->CreateProgram("KleberTriangleBlob-Copy");
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();
    fileLoadProperties.mPath = VERTEX_SHADER;
    Pegasus::Shader::ShaderStageRef vertexShader = shaderManager->LoadShaderStageFromFile(fileLoadProperties);    
    fileLoadProperties.mPath = FRAGMENT_SHADER;
    Pegasus::Shader::ShaderStageRef fragmentShader = shaderManager->LoadShaderStageFromFile(fileLoadProperties);
    mProgram->SetShaderStage( vertexShader );
    mProgram->SetShaderStage( fragmentShader );

    mProgram2->SetShaderStage( vertexShader );
    mProgram2->SetShaderStage( fragmentShader );


    // Force a compilation of the shaders
    bool updated = false;
    mProgram->GetUpdatedData(updated);

    // Set up shader uniforms
    mTimeUniform = 0;//glGetUniformLocation(mProgramData->GetHandle(), "time");

    Pegasus::Mesh::MeshGeneratorRef quadGenerator = GetMeshManager()->CreateMeshGeneratorNode("QuadGenerator");
    mQuadMesh = GetMeshManager()->CreateMeshNode();
    mQuadMesh->SetGeneratorInput(quadGenerator);
    mQuadMesh->GetUpdatedMeshData();
}

//----------------------------------------------------------------------------------------

void KleberTriangleBlock::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
}

//----------------------------------------------------------------------------------------

void KleberTriangleBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::Dispatch(mProgram);
    Pegasus::Render::Dispatch(mQuadMesh);
    const float currentTime = beat * 30.0f;
    glUniform1f(mTimeUniform, currentTime);
    Pegasus::Render::Draw();
}
