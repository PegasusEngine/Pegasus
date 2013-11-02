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
namespace Window {

StartupWindow::StartupWindow(const Pegasus::Window::WindowConfig& config)
    : Pegasus::Window::Window(config)
{
}

//----------------------------------------------------------------------------------------

StartupWindow::~StartupWindow()
{
}

//----------------------------------------------------------------------------------------

Pegasus::Window::Window* StartupWindow::Create(const Pegasus::Window::WindowConfig& config)
{
    return PG_NEW("StartupWindow", Pegasus::Memory::PG_MEM_PERM) StartupWindow(config);
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

void StartupWindow::Refresh()
{
    // Flip the GPU
    GetRenderContext()->Swap();
}


} // end namespace Window
} // end namesapce Pegasus
