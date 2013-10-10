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
#include "Pegasus\Application\ApplicationProxy.h"


// move these guys into files once we have this working
#include <stdlib.h>


const char * VERTEX_SHADER_SRC = \
"#version 330 core                \n"
"layout(location = 0) in vec3 vPos; "
"out vec4 screenPos;                "
"uniform float time;               "
"void main()                        "
"{                                  "
"    vec4 aspect = vec4(960.0f/540.0f, 1,1,1);        "
"    gl_Position = aspect*vec4(vPos + vec3(0.2*sin(time*0.001),0.4*cos(time*0.001), 0) , 1.0f);"
"    float st = sin(time*0.005);"
"    float ct = cos(time*0.005);"
"    vec4 r = vec4(st,ct,-ct,st);"
"    gl_Position.xy = vec2(dot(gl_Position.xy,r.xy),dot(gl_Position.xy,r.zw));"
"    screenPos = gl_Position ;       "
"}                                  "
;


const char * FRAGMENT_SHADER_SRC = \
"#version 330 core                \n"
"in  vec4 screenPos;"
"out vec4 color;                     "
"uniform float time;"
"vec3 twist(vec3 p)"
"{"
"    p = vec3(0.3*sin((p.y+time*0.001)*2.7)+p.x,p.y,0.3*cos((p.y+time*0.001)*2.2)+p.z) ;"
"    return p;"
"}"
"void main()                        "
"{                                  "
"     vec3 camPos = vec3(0,0,-1);   "
"     vec3 camDir = normalize(screenPos.xyz - camPos);"
"     vec3 p = camPos;              "
"     float d = 10000;"
"     int j = 0;      "
"     for (int i = 0; i < 32; ++i)"
"     {"
"        const float radius = 0.7;"
"         vec3 twisted = twist(p);"
"        d = length(vec3(0,0,0.9) - twisted) - 0.7;"
"        if (d < 0.001)"
"        {"
"            break;"
"        }"
"        else"
"        {"
"            p += camDir * d;"
"            j = i; "
"        }"
"     }"
"    vec4 sphere = (d < 0.001 ? 1.0f : 0.0f) * vec4(0.7f * float(j)/32.0f + 0.2f, float(j)/32.0f + 0.1f,0,0);"
"    color = vec4(0,0,0.2,1) + sphere ;"
"}                                  "
;


TestApp1::TestApp1()
:   Pegasus::Application::Application(),
    mViewportWidth(128),
    mViewportHeight(128),
    mFrame(0)
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

void TestApp1::HandleShaderError(GLuint object, const char * errorMessage) const
{
    const GLsizei bufferSize = 256;
    char buffer[bufferSize];
    GLsizei logLength = 0;
    glGetShaderInfoLog(object, bufferSize, &logLength, buffer);
    PG_FAILSTR("[Shader]%s: %s", errorMessage, buffer);
}


void TestApp1::HandleProgramError(GLuint object, const char * errorMessage) const
{
    const GLsizei bufferSize = 256;
    char buffer[bufferSize];
    GLsizei logLength = 0;
    glGetProgramInfoLog(object, bufferSize, &logLength, buffer);
    PG_FAILSTR("[Program]%s: %s", errorMessage, buffer);
}

GLuint TestApp1::CompileShaderSource(const char * src, GLenum shaderType) const
{
    GLuint shaderName = glCreateShader(shaderType);    
    glShaderSource(
        shaderName,
        1, // count
        &src, // src (list of strings, on this case just 1 string
        NULL // length of each string in the list of strings,
            //opengl samples use NULL, not nullptr
            // NULL in this argument means just assume each string is a NULL terminated string
    );
    glCompileShader(shaderName);
    GLint shaderCompiled = GL_TRUE;
    glGetShaderiv(shaderName, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_TRUE)
    {
        //successful compilation!
        return shaderName;
    }
    else
    {
        //failure shenanigans
        HandleShaderError(shaderName, "failed shader compilation!");
    }
    return 0;
} 

GLuint TestApp1::CreateVertexBuffer(GLfloat * vertices, GLuint size) const
{
    GLuint bufferName;
    glGenBuffers(1, &bufferName);    
    glBindBuffer(GL_ARRAY_BUFFER, bufferName);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) *  size, vertices, GL_STATIC_DRAW); 
    return bufferName;
}

GLuint TestApp1::CreateIndexBuffer(GLushort * indices, GLuint size) const
{
    GLuint bufferName;
    glGenBuffers(1, &bufferName);    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * size, indices, GL_STATIC_DRAW); 
    return bufferName;
}

GLuint TestApp1::CreateProgram(const char * vertexShaderSource, const char * fragmentShaderSource) const
{
    GLuint vertexShaderName = CompileShaderSource(vertexShaderSource, GL_VERTEX_SHADER);       
    GLuint fragmentShaderName = CompileShaderSource(fragmentShaderSource, GL_FRAGMENT_SHADER);       
    if (vertexShaderName != 0 && fragmentShaderName != 0)
    {
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShaderName);
        glAttachShader(shaderProgram, fragmentShaderName);
        glLinkProgram(shaderProgram);
        GLint shaderLinked = GL_FALSE;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderLinked);
        if (shaderLinked == GL_TRUE)
        {
            return shaderProgram;
        }
        else
        {
            HandleProgramError(shaderProgram, "failed shader linking!");
        }
    }
    return 0;
}

void TestApp1::OnWindowReady(int windowId)
{
    // for now only the first window gets proper rendering
    if (windowId == 0)
    {
        GLushort indices[] = {0, 1, 2};
        GLfloat  vertexPositions[] = 
        {
          -0.7f, -0.7f,
          0.7f, -0.7f,
           0.0f, 0.9f
        };

        mRenderContextData.mProgram = CreateProgram(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);

        //create the maverick index buffer
        mRenderContextData.mIndexBuffer = CreateIndexBuffer(indices, sizeof(indices));
    
        glGenVertexArrays(1, &mRenderContextData.mVertexArray); //this is the actual holder of our vertex stream
                                                                //each vertex array represents a mesh, which could
                                                                //be composed of several vertex buffers
        glBindVertexArray(mRenderContextData.mVertexArray);//the next operations use this vertex array.

        mRenderContextData.mVertexBuffer = CreateVertexBuffer(vertexPositions, sizeof(vertexPositions));

        if (mRenderContextData.mProgram != 0)
        {
            glUseProgram(mRenderContextData.mProgram);
            mRenderContextData.mTimeUniform = glGetUniformLocation(mRenderContextData.mProgram, "time");
            const int positionLocation = 0;
            glVertexAttribPointer(
                positionLocation, //layout location
                2, //the size of each element, this case only 2
                   //notice the shader has a vec4 as input
                   //opengl fills the rest with defaults, so in vec4 = (0,0,0,1);
                GL_FLOAT, //the type
                GL_FALSE, //not normalized
                0,//stride, meaning we are tightly packed
                0//offset, we start at the first byte, 
            );
            glEnableVertexAttribArray(positionLocation);
        }

    }
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
    glViewport(0, 0, mViewportWidth, mViewportHeight);

    mFrame++;
    // Clear
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mRenderContextData.mProgram);
    glUniform1f(mRenderContextData.mTimeUniform, static_cast<float>(mFrame));
    glBindVertexArray(mRenderContextData.mVertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRenderContextData.mIndexBuffer);
    glDrawElements(
        GL_TRIANGLE_STRIP, //the type to draw
        3,
        GL_UNSIGNED_SHORT,
        0 //no offset
    );

    // Call into base to do present
    // \todo Clean this up
    Application::Render(); // swap
    glFlush(); //flush gpu
}
