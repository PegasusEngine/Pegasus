/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                   */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.cpp
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1

#include "TestApp1.h"

#define GLEW_STATIC 1
#include "Pegasus/Libs/GLEW/glew.h"

#include <stdlib.h>

// Statics / globals
const GLuint NUM_VERTS = 3;
enum VAO_IDs { TRIANGLES = 0, NUM_VAO };
enum Buffer_IDs { TRIANGLE_ARRAYBUFFER = 0, NUM_BUFFERS};
enum Attrib_IDs { vPosition = 0 };
bool TestApp1::sAppInitted = false;

// Triangle objects
GLuint VAOs[NUM_VAO];
GLuint Buffers[NUM_BUFFERS];
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

// Shaders
const char* TRIANGLES_VERT =
    "#version 430 core\n"
    "layout (location = 0) in vec4 vPosition;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vPosition;\n"
    "}\n";
const char* TRIANGLES_FRAG =
    "#version 430 core\n"
    "out vec4 fColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    fColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
    "}\n";

TestApp1::TestApp1()
:   Pegasus::Application::Application(),
    mViewportWidth(128),
    mViewportHeight(128)
{
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{

}

//----------------------------------------------------------------------------------------

void TestApp1::Initialize(const Pegasus::Application::ApplicationConfig& config)
{
    Pegasus::Application::Application::Initialize(config);
}

//----------------------------------------------------------------------------------------

void TestApp1::Shutdown()
{
    Pegasus::Application::Application::Shutdown();
}

//----------------------------------------------------------------------------------------

void TestApp1::Resize(const Pegasus::Window::Window * wnd, int width, int height)
{
    mViewportWidth = width;
    mViewportHeight = height;
}

//----------------------------------------------------------------------------------------

void TestApp1::Render()
{
    // \todo refactor this nonsense
    if (!sAppInitted)
    {
        InitRendering();
        sAppInitted = true;
    }

    RenderFrame(0.0f); // \todo Input time

    // Call into base to do present
    // \todo Clean this up
    Application::Render();
}

void TestApp1::InitRendering()
{
    GLsizei buffSize;
    char infoLog[4096];
    const GLfloat verts[NUM_VERTS][2] = {
        { -0.6f, -0.6f },
        { 0.6f, -0.6f },
        { 0.0f, 0.6f }
    };

    // Create and bind vertex array
    glGenVertexArrays(NUM_VAO, VAOs);
    glBindVertexArray(VAOs[TRIANGLES]);

    // Create and fill buffers
    glGenBuffers(NUM_BUFFERS, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TRIANGLE_ARRAYBUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Immutable verts

    // Set up shaders
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();
    glShaderSource(vertexShader, 1, &TRIANGLES_VERT, NULL);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader, 4096, &buffSize, infoLog);
    glShaderSource(fragmentShader, 1, &TRIANGLES_FRAG, NULL);
    glCompileShader(fragmentShader);
    glGetShaderInfoLog(vertexShader, 4096, &buffSize, infoLog);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramInfoLog(vertexShader, 4096, &buffSize, infoLog);
    glUseProgram(shaderProgram);

    // Bind vertex array to shader
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); // 2 floats, non-normalized, 0 stride and offset
    glEnableVertexAttribArray(vPosition);
}

void TestApp1::RenderFrame(float time)
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up and draw triangles
    glBindVertexArray(VAOs[TRIANGLES]);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);

    // Kick the GPU
    glFlush();
}

