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

//#define GLEW_STATIC 1
#include "Pegasus/Libs/GLEW/glew.h"

#include <stdlib.h>

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
    glViewport(0, 0, mViewportWidth, mViewportHeight);

    static unsigned int counter = 0;
    static float red = 0.0f;
    static float green = 0.0f;
    static float blue = 0.0f;

    // Check timer
    if (counter == 0)
    {
        red = (float) rand() / (float) RAND_MAX;
        green = (float) rand() / (float) RAND_MAX;
        blue = (float) rand() / (float) RAND_MAX;
    }
    counter = (counter + 1) % 60;

    // Clear
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Call into base to do present
    // \todo Clean this up
    Application::Render();
}
