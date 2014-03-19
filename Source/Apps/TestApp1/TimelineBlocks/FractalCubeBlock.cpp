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


static const GLuint NUM_VERTS = 6;
static const GLfloat verts[NUM_VERTS][2] =
    {
        { -1.0f, -1.0f },
        {  1.0f, -1.0f },
        { -1.0f,  1.0f },
        { -1.0f,  1.0f },
        {  1.0f, -1.0f },
        {  1.0f,  1.0f }
    };
enum ATTRIB_IDS { POSITION_ATTRIB = 0 };

static const char * VERTEX_SHADER = "Shaders\\Cubefractal.vs";
static const char * FRAGMENT_SHADER = "Shaders\\CubeFractal.ps";

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
    // Create and bind vertex array
    glGenVertexArrays(NUM_VAOS, mVAOs);
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);

    // Create and fill buffers
    glGenBuffers(NUM_BUFFERS, mBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TRIANGLES_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Immutable verts

    // Set up shaders
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mShaderProgramLinkage = shaderManager->CreateProgram("FractalCube");
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();

    fileLoadProperties.mPath = VERTEX_SHADER;
    mShaderProgramLinkage->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    fileLoadProperties.mPath = FRAGMENT_SHADER;
    mShaderProgramLinkage->SetShaderStage( shaderManager->LoadShaderStageFromFile(fileLoadProperties) );

    // Force a compilation of the shaders
    bool updated = false;
    mProgramData = mShaderProgramLinkage->GetUpdatedData(updated);

    // Use the shader
    //! \todo Why do we know that here?
    mProgramData->Use();

    // Set up shader uniforms
    mTimeUniform = glGetUniformLocation(mProgramData->GetHandle(), "time");
    mScreenRatioUniform = glGetUniformLocation(mProgramData->GetHandle(), "screenRatio");

    // Bind vertex array to shader
    glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); // 2 floats, non-normalized, 0 stride and offset
    glEnableVertexAttribArray(POSITION_ATTRIB);
}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Shutdown()
{
    //! \todo Uninitialize VAOs, buffers, shaders
}

//----------------------------------------------------------------------------------------

void FractalCubeBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    // Make sure the shaders are compiled
    //! \todo Why do we need this?
    bool updated = false;
    mProgramData = mShaderProgramLinkage->GetUpdatedData(updated);

    // Use the shader
    mProgramData->Use();

    const float currentTime = beat * 0.25f;
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);

    // Set up and draw triangles
    glBindVertexArray(mVAOs[TRIANGLES_VAO]);
    glUniform1f(mTimeUniform, currentTime);
    glUniform1f(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
}
