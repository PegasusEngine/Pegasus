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
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

#if DEMO == DEMO_KLEBER_HOMOGAY_TRIANGLE

// Shaders
const char* TRIANGLES_VERT =
"#version 430 core                \n"
"//HomoGay Triangle vx, by Kleber     \n "
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
"}                                ";

const char* TRIANGLES_FRAG =
    "#version 430 core\n"
    "//HomoGay Triangle fx, by Kleber     \n "
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
    "} ";

#endif  // DEMO_KLEBER_HOMOGAY_TRIANGLE

#if DEMO == DEMO_KEVIN_PSYBEADS

const char* TRIANGLES_VERT = "\
#version 330 core\n\
layout(location = 0) in vec2 vPos;\
out vec3 p,d;\
uniform float time;\
uniform float screenRatio;\
void main(){\
    gl_Position=vec4(vPos.x,vPos.y*screenRatio,0.0,1.0);\
    p=3.0*vec3(cos(time),sin(time),time);\
    d=vec3(vPos.x*p.x-vPos.y*p.y,vPos.y*p.x+vPos.x*p.y,1);\
}";

const char* TRIANGLES_FRAG = "\
#version 330 core\n\
in vec3 p,d;\
out vec4 color;\
void main()\
{\
    vec3 e=normalize(d),r;\
    float f,t=0;\
    int i=0;\
    do\
    {\
        r=fract(e*t+vec3(p.xy,0))-.5;\
        f=min(.25*(length(r.yz)+length(r.xy)),length(r.xz))-.1;\
        t+=.5*f;\
    }\
    while(f>.01&&i++<100);\
    f=sin(t);\
    color=vec4(f*.5+.2+smoothstep(.6,.62,i*.01)*smoothstep(.64,.62,i*.01),f*.6+.2,f*.7+.3,1);\
}";

#endif  // DEMO_KEVIN_PSYBEADS

#if DEMO == DEMO_KEVIN_CUBE_FRACTAL

const char* TRIANGLES_VERT = "\
#version 330 core\n\
layout(location = 0) in vec2 vPos;\
out vec3 p,d;\
uniform float time;\
uniform float screenRatio;\
void main(){\
    gl_Position=vec4(vPos.x,vPos.y*screenRatio,0.0,1.0);\
    p = vec3(0,0,time);\
    d = vec3(vPos.x * cos(time) + sin(time), vPos.y, cos(time) - vPos.x * sin(time));\
}";

const char* TRIANGLES_FRAG = "\
#version 330 core\n\
in vec3 p,d;\
out vec4 color;\
void main()\
{\
	vec3 q,r;\
	float f=0,g=0,k=0,c=0,t=0;\
	int i=0,j;\
	do\
	{\
		f=-1;\
		k=1;\
		for(j=0;j<6;++j)\
		{\
			q = ((1/6.) - abs(fract((d*t + p) * k + .5) - .5)) / k;\
			r = min(q,q.yzx);\
			g = max(r.x, max(r.y,r.z));\
			if(g>f)\
			{\
				f=g;\
				c=d.x;\
				if(g==q.y) c=d.y;\
				if(g==q.z) c=d.z;\
			}\
			/*k*=3;*/\
			/*k*=2;*/\
            /*k*=1.73;*/\
            k*=1.7;\
		}\
		t+=.7*f;\
	}\
	while(f>.0006 && i++<100);\
    color = vec4(abs(c) / (1+t) / (1.+float(i)*.05));\
}";

#endif  // DEMO_KEVIN_CUBE_FRACTAL

#if DEMO == DEMO_KEVIN_CUBE_FRACTAL2

const char* TRIANGLES_VERT = "\
#version 330 core\n\
layout(location = 0) in vec2 vPos;\
out vec3 p,d;\
uniform float time;\
uniform float screenRatio;\
void main(){\
    gl_Position=vec4(vPos.x,vPos.y*screenRatio,0.0,1.0);\
    p = vec3(0,0,time*.6);\
    d = vec3(vPos.x * cos(time) + sin(time), vPos.y + .4*sin(time), cos(time) - vPos.x * sin(time));\
}";

const char* TRIANGLES_FRAG = "\
#version 330 core\n\
in vec3 p,d;\
out vec4 color;\
void main()\
{\
	vec3 q,r;\
	float f=0,g=0,k=0,t=0;\
    vec3 c=vec3(0);\
	int i=0,j;\
	do\
	{\
		f=-1;\
		k=1;\
		for(j=0;j<6;++j)\
		{\
			q = ((1/6.) - abs(fract((d*t + p) * k + .5) - .5)) / k;\
			r = min(q,q.yzx);\
			g = max(r.x, max(r.y,r.z));\
			if(g>f)\
			{\
				f=g;\
                c=d.x*vec3(1,.8,.8);\
				if(g==q.y) c=d.y*vec3(.8,1,.8);\
				if(g==q.z) c=d.z*vec3(.8,.8,1);\
			}\
			/*k*=3;*/\
			/*k*=2;*/\
            /*k*=1.73;*/\
            k*=1.7;\
		}\
		t+=.7*f;\
	}\
	while(f>.0006 && i++<100);\
    color = vec4(abs(c) / (1+t) / (1.+float(i)*.05), 1);\
    \
    t-=.72*f;\
    vec3 ip=(d*t+p);\
    vec3 lp=p;\
    lp.z += .3*d.x;\
    vec3 ld=normalize(ip-lp);\
    t=0;\
    i=0;\
    do\
	{\
		f=-1;\
		k=1;\
		for(j=0;j<6;++j)\
		{\
			q = ((1/6.) - abs(fract((ld*t + lp) * k + .5) - .5)) / k;\
			r = min(q,q.yzx);\
			g = max(r.x, max(r.y,r.z));\
            f = max(f,g);\
            k*=1.7;\
		}\
		t+=.7*f;\
	}\
	while(f>.0006 && i++<70);\
    t-=.7*f;\
    if (t < .9*length(ip-lp))\
        color.xyz *= .4;\
    color.xyz *= .8+.2*fract(.5+ip*10.);\
}";

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
    GLsizei buffSize;
    char infoLog[4096];

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
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();
    glShaderSource(vertexShader, 1, &TRIANGLES_VERT, NULL);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader, 4096, &buffSize, infoLog);
    PG_ASSERTSTR(infoLog[0] == '\0', "Invalid vertex shader");
    glShaderSource(fragmentShader, 1, &TRIANGLES_FRAG, NULL);
    glCompileShader(fragmentShader);
    glGetShaderInfoLog(fragmentShader, 4096, &buffSize, infoLog);
    PG_ASSERTSTR(infoLog[0] == '\0', "Invalid fragment shader");
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramInfoLog(shaderProgram, 4096, &buffSize, infoLog);
    PG_ASSERTSTR(infoLog[0] == '\0', "Invalid shader program");
    glUseProgram(shaderProgram);
    mTimeUniform = glGetUniformLocation(shaderProgram, "time");
#if (DEMO == DEMO_KEVIN_PSYBEADS) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL) || (DEMO == DEMO_KEVIN_CUBE_FRACTAL2)
    mScreenRatioUniform = glGetUniformLocation(shaderProgram, "screenRatio");
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

