/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   StartupWindow.cpp
//! \author David Worsham
//! \date   20th October 2013
//! \brief  Specialized window for the internal startup window.

#include "Pegasus/Window/StartupWindow.h"
#include "Pegasus/Render/RenderContext.h"

namespace Pegasus {
namespace Wnd {

StartupWindow::StartupWindow(const Pegasus::Wnd::WindowConfig& config)
    : Pegasus::Wnd::Window(config)
{
}

//----------------------------------------------------------------------------------------

StartupWindow::~StartupWindow()
{
}

//----------------------------------------------------------------------------------------

Pegasus::Wnd::Window* StartupWindow::Create(const Pegasus::Wnd::WindowConfig& config, Alloc::IAllocator* alloc)
{
    return PG_NEW(alloc, -1, "StartupWindow", Pegasus::Alloc::PG_MEM_PERM) StartupWindow(config);
}

//----------------------------------------------------------------------------------------

void StartupWindow::Initialize()
{
}

//----------------------------------------------------------------------------------------

void StartupWindow::Shutdown()
{
}

//----------------------------------------------------------------------------------------

void StartupWindow::Render()
{
    // Flip the GPU
    GetRenderContext()->Swap();
}


} // end namespace Wnd
} // end namespace Pegasus
