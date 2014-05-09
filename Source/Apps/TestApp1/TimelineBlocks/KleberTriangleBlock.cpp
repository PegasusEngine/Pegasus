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
#include "Pegasus/Render/Render.h"


static const GLuint NUM_VERTS = 3;
static const GLfloat verts[NUM_VERTS][2] =
    {
        { -0.6f, -0.6f },
        { 0.6f, -0.6f },
        { 0.0f, 0.6f }
    };
enum ATTRIB_IDS { POSITION_ATTRIB = 0 };

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
    // Create and bind vertex array
    glGenVertexArrays(NUM_VAOS, mVAOs);
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);

    // Create and fill buffers
    glGenBuffers(NUM_BUFFERS, mBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TRIANGLES_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Immutable verts

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

    // Bind vertex array to shader
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); // 2 floats, non-normalized, 0 stride and offset
    glEnableVertexAttribArray(POSITION_ATTRIB);
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

    const float currentTime = beat * 30.0f;

    // Set up and draw triangles
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);
    glUniform1f(mTimeUniform, currentTime);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
}
