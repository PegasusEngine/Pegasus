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
#include "Pegasus/Core/IApplicationContext.h"
#include "TestApp1Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Render/Render.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Window/Shared/WindowConfig.h"
#include "Pegasus/Allocator/IAllocator.h"

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
}

