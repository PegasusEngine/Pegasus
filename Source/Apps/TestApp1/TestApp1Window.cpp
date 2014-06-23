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
#include "Pegasus/Render/Render.h"

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
    
    // set default render target
    Pegasus::Render::Viewport regularViewport(viewportWidth, viewportHeight);
    Pegasus::Render::DispatchDefaultRenderTarget(regularViewport);

    // set clear color and depth
    Pegasus::Render::SetClearColorValue(Pegasus::Math::ColorRGBA(0.0, 0.0, 0.0, 1.0));

    // - todo inverted depth
    Pegasus::Render::SetDepthClearValue(0.0);
    
    // clear buffers
    Pegasus::Render::Clear(/*color*/true, /*depth*/ true, /*stencil*/false);

    // Render the content of the timeline
    GetWindowContext()->GetTimeline()->Render(this);

    // Flip the GPU
    GetRenderContext()->Swap();
}

