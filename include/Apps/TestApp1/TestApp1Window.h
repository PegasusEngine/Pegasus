/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1Window.h
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Specialized window for TestApp1.

#ifndef TESTAPP1WINDOW_H
#define TESTAPP1WINDOW_H

#include "Pegasus/Pegasus.h"
#include "Pegasus/Shader/Shader.h"


//! \class This is a specialized window for the TestApp1 application main window.  Place
//         specialized rendering code here.
class TestApp1Window : public Pegasus::Window::Window
{
public:
    // Ctor / dtor
    TestApp1Window(const Pegasus::Window::WindowConfig& config);
    ~TestApp1Window();

    // Factory API
    static Pegasus::Window::Window* Create(const Pegasus::Window::WindowConfig& config);

    // App-specific API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Refresh();

private:
    Pegasus::Shader::ShaderStage mVertexShader;
    Pegasus::Shader::ShaderStage mFragmentShader;
    Pegasus::Shader::Program     mShaderProgram;

    // Rendering stuff
    int mTimeUniform;
    int mScreenRatioUniform;
    int mFrame;
};

#endif  // TESTAPP1WINDOW_H
