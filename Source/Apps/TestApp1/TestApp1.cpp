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
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
static const char* SECONDARY_WND_TYPE = "TestApp1SecondaryWindow";
#endif

TestApp1::TestApp1(const Pegasus::App::ApplicationConfig& config)
:   Pegasus::App::Application(config)
{
    Pegasus::App::WindowRegistration reg;

    // Register the main window
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_MAIN;
    reg.mDescription = "TestApp1 Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(MAIN_WND_TYPE, reg);

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    // Register the secondary window
    //! \todo That is weird to have to register that window here.
    //!       Also, where should the texture and mesh viewers be created?
    //! \todo Is it normal to have to use a different type tag?
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_SECONDARY;
    reg.mDescription = "TestApp1 Secondary Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(SECONDARY_WND_TYPE, reg);
#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{
    // Unregister the main window
    GetWindowRegistry()->UnregisterWindowClass(MAIN_WND_TYPE);
}

//----------------------------------------------------------------------------------------

Pegasus::App::Application * CreateApplication(const Pegasus::App::ApplicationConfig& config)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    return PG_NEW(globalAlloc, -1, "TestApp1", Pegasus::Alloc::PG_MEM_PERM) TestApp1(config);
}

//----------------------------------------------------------------------------------------

void DestroyApplication(Pegasus::App::Application* app)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    PG_DELETE(globalAlloc, app);
}
