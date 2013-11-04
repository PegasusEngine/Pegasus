/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                   */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.cpp
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1
//! \todo Have the app wizard include appexport and set up this file correctly

#include "TestApp1.h"
#include "TestApp1Window.h"
#include "Pegasus/AppExport.h" // Needs including, for exported functions.  Only include this once!

// Variables
static const char* MAIN_WND_TYPE = "TestApp1Window";

TestApp1::TestApp1(const Pegasus::App::ApplicationConfig& config)
:   Pegasus::App::Application(config)
{
    Pegasus::App::WindowRegistration reg;

    // Register the main window
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_MAIN;
    reg.mDescription = "TestApp1 Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(MAIN_WND_TYPE, reg);
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{
    // Deregister the main window
    GetWindowRegistry()->UnregisterWindowClass(MAIN_WND_TYPE);
}

//----------------------------------------------------------------------------------------

Pegasus::App::Application * CreateApplication(const Pegasus::App::ApplicationConfig& config)
{
    Pegasus::Memory::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    return PG_NEW(globalAlloc, "TestApp1", Pegasus::Memory::PG_MEM_PERM) TestApp1(config);
}

//----------------------------------------------------------------------------------------

void DestroyApplication(Pegasus::App::Application* app)
{
    Pegasus::Memory::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    PG_DELETE(globalAlloc, app);
}
