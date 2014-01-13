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


TestApp1Window::TestApp1Window(const Pegasus::Wnd::WindowConfig& config)
    : Pegasus::Wnd::Window(config), mAllocator(config.mAllocator)
{
}

//----------------------------------------------------------------------------------------

TestApp1Window::~TestApp1Window()
{
}

//----------------------------------------------------------------------------------------

Pegasus::Wnd::Window* TestApp1Window::Create(const Pegasus::Wnd::WindowConfig& config, Pegasus::Alloc::IAllocator* alloc)
{
    return PG_NEW(alloc, -1, "TestApp1Window", Pegasus::Alloc::PG_MEM_PERM) TestApp1Window(config);
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Initialize()
{
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Shutdown()
{
}

//----------------------------------------------------------------------------------------

void TestApp1Window::Render()
{
    // Set up rendering
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    GetDimensions(viewportWidth, viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the content of the timeline
    GetWindowContext()->GetTimeline()->Render(this);

    // Flip the GPU
    GetRenderContext()->Swap();
}

