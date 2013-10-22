/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1Window.cpp
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Specialized window for TestApp1.

#define GLEW_STATIC 1
#include "TestApp1Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Libs/GLEW/glew.h"

#include <stdlib.h>

//! \todo Temporary, just to get some animation running
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Statics / globals
// Demo to execute
#define DEMO_KLEBER_HOMOGAY_TRIANGLE    1
#define DEMO_KEVIN_PSYBEADS             2
#define DEMO_KEVIN_CUBE_FRACTAL         3
#define DEMO_KEVIN_CUBE_FRACTAL2        4
#define DEMO                            DEMO_KEVIN_CUBE_FRACTAL2
#define DEMO_ROOT  "..\\..\\..\\..\\Data\\AppTest1\\Imported\\Shaders\\"

// Statics / globals
#if DEMO == DEMO_KLEBER_HOMOGAY_TRIANGLE
const GLuint NUM_VERTS = 3;
#endif
#if (DEMO == DEMO_KEVIN_PSYBEADS) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL2)
const GLuint NUM_VERTS = 6;
#endif
enum VAO_IDs { TRIANGLES = 0, NUM_VAO };
enum Buffer_IDs { TRIANGLE_ARRAYBUFFER = 0, NUM_BUFFERS};
enum Attrib_IDs { vPosition = 0 };

// Triangle objects
GLuint VAOs[NUM_VAO];
GLuint Buffers[NUM_BUFFERS];



#if DEMO == DEMO_KLEBER_HOMOGAY_TRIANGLE
    const char* TRIANGLES_VERT = DEMO_ROOT "Blob.vs";
    const char* TRIANGLES_FRAG = DEMO_ROOT "Blob.ps";
#endif  // DEMO_KLEBER_HOMOGAY_TRIANGLE

#if DEMO == DEMO_KEVIN_PSYBEADS
    const char* TRIANGLES_VERT = DEMO_ROOT "Psybeads.vs";
    const char* TRIANGLES_FRAG = DEMO_ROOT "Psybeads.ps";
#endif  // DEMO_KEVIN_PSYBEADS

#if DEMO == DEMO_KEVIN_CUBE_FRACTAL
    const char* TRIANGLES_VERT = DEMO_ROOT "Cubefractal.vs";
    const char* TRIANGLES_FRAG = DEMO_ROOT "CubeFractal.ps";
#endif  // DEMO_KEVIN_CUBE_FRACTAL

#if DEMO == DEMO_KEVIN_CUBE_FRACTAL2
    const char* TRIANGLES_VERT = DEMO_ROOT "Cubefractal2.vs";
    const char* TRIANGLES_FRAG = DEMO_ROOT "CubeFractal2.ps";
#endif  // DEMO_KEVIN_CUBE_FRACTAL2


TestApp1Window::TestApp1Window(const Pegasus::Window::WindowConfig& config)
    : Pegasus::Window::Window(config)
{
}

//----------------------------------------------------------------------------------------

TestApp1Window::~TestApp1Window()
{
}

//----------------------------------------------------------------------------------------

Pegasus::Window::Window* TestApp1Window::Create(const Pegasus::Window::WindowConfig& config)
{
    return PG_CORE_NEW("TestApp1Window", Pegasus::Memory::PG_MEM_PERM) TestApp1Window(config);
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Initialize()
{
#if DEMO == DEMO_KLEBER_HOMOGAY_TRIANGLE
    const GLfloat verts[NUM_VERTS][2] = {
        { -0.6f, -0.6f },
        { 0.6f, -0.6f },
        { 0.0f, 0.6f }
    };
#endif
#if (DEMO == DEMO_KEVIN_PSYBEADS) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL2)
    const GLfloat verts[NUM_VERTS][2] = {
        { -1.0f, -1.0f },
        {  1.0f, -1.0f },
        { -1.0f,  1.0f },
        { -1.0f,  1.0f },
        {  1.0f, -1.0f },
        {  1.0f,  1.0f }
    };
#endif

    // Create and bind vertex array
    glGenVertexArrays(NUM_VAO, VAOs);
    glBindVertexArray(VAOs[TRIANGLES]);

    // Create and fill buffers
    glGenBuffers(NUM_BUFFERS, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TRIANGLE_ARRAYBUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Immutable verts

    // Set up shaders
    mVertexShader.CompileFromFile(TRIANGLES_VERT);
    mFragmentShader.CompileFromFile(TRIANGLES_FRAG);
    mShaderProgram.SetStage(&mVertexShader);
    mShaderProgram.SetStage(&mFragmentShader);
    mShaderProgram.Link();
    
    //use the shader
    mShaderProgram.Bind();

    mTimeUniform = glGetUniformLocation(mShaderProgram.GetProgramHandle(), "time");
#if (DEMO == DEMO_KEVIN_PSYBEADS) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL2)
    mScreenRatioUniform = glGetUniformLocation(mShaderProgram.GetProgramHandle(), "screenRatio");
#endif
    mFrame = 0;
    // Bind vertex array to shader
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); // 2 floats, non-normalized, 0 stride and offset
    glEnableVertexAttribArray(vPosition);
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Shutdown()
{
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Refresh()
{
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;

    // Set up rendering
    GetDimensions(viewportWidth, viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    mFrame = (int) (GetApplication()->GetAppTime() * 60.0f); // Time is in s

#if DEMO == DEMO_KLEBER_HOMOGAY_TRIANGLE
    //! \todo Temporary, just to get some animation running
    const float currentTime = (static_cast<float>(GetTickCount()) * 0.001f) * 60.0f;

    // Set up and draw triangles
    glBindVertexArray(VAOs[TRIANGLES]);
    glUniform1f(mTimeUniform, currentTime);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
#endif  // DEMO_KLEBER_HOMOGAY_TRIANGLE

#if (DEMO == DEMO_KEVIN_PSYBEADS) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL2)
    //! \todo Temporary, just to get some animation running
    const float currentTime = (static_cast<float>(GetTickCount()) * 0.001f) * 0.5f;

    // Set up and draw triangles
    glBindVertexArray(VAOs[TRIANGLES]);
    glUniform1f(mTimeUniform, currentTime);
    glUniform1f(mScreenRatioUniform, static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
#endif  // DEMO_KEVIN_PSYBEADS

    // Flip the GPU
    GetRenderContext()->Swap();
}

