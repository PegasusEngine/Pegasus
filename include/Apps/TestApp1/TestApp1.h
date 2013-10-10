/****************************************************************************************/
/*                                                                                      */
/*                                  Pegasus Application                                 */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.h
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1

#ifndef TESTAPP1_H
#define TESTAPP1_H

#include "Pegasus/Pegasus.h" 
#define GLEW_STATIC 1
#include "Pegasus/Libs/GLEW/glew.h"

//small struct to keep track of context related data

struct TestApp1RenderContextData
{
    GLuint mProgram; 
    GLuint mVertexArray;
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;
    GLuint mTimeUniform;
    TestApp1RenderContextData()
    : mProgram(0)
    {
    }
};

class TestApp1 : public Pegasus::Application::Application
{
public:
    // Ctor / dtor
    TestApp1();
    virtual ~TestApp1();

    // App API
    virtual void Initialize(const Pegasus::Application::ApplicationConfig& config);
    virtual void Shutdown();
    virtual void Resize(const Pegasus::Window::Window * wnd, int width, int height);
    virtual void Render();

    // Application callback implementations
    virtual void OnWindowReady(int id);

    //functions that contain boiler plate code for shader initialization
    //these can go into our opengl Rendering library
    GLuint CompileShaderSource(const char * src, GLenum type) const;
    void   HandleShaderError(GLuint object, const char * message) const;
    void   HandleProgramError(GLuint object, const char * message) const;
    GLuint CreateProgram(const char * vertexShaderSource, const char * fragmentShaderSource) const;
    GLuint CreateVertexBuffer(GLfloat * vertices, GLuint length) const;
    GLuint CreateIndexBuffer(GLushort * indices, GLuint length) const;
    

private:

    int mViewportWidth;
    int mViewportHeight;
    int mFrame;
    TestApp1RenderContextData mRenderContextData;    
};


//! \todo Make this a macro with only the name of the app class as parameter
Pegasus::Application::Application * CreateApplication() { return new TestApp1(); }
void DestroyApplication(Pegasus::Application::Application* app) { delete app; }

#endif  // TESTAPP1_H
