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
static const char* MAIN_WINDOW_TYPE = "TestApp1Window";

TestApp1::TestApp1(const Pegasus::Application::ApplicationConfig& config)
:   Pegasus::Application::Application(config)
{
    Pegasus::Application::WindowRegistration reg;

    // Register the main window
    reg.mTypeTag = Pegasus::Application::WINDOW_TYPE_MAIN;
    reg.mDescription = "TestApp1 Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(MAIN_WINDOW_TYPE, reg);
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{
    // Deregister the main window
    GetWindowRegistry()->UnregisterWindowClass(MAIN_WINDOW_TYPE);
}
