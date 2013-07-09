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
//! \todo Remove the include once OutputDebugString below is not used
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


TestApp1::TestApp1()
:   Pegasus::Application()
{
    // Test for preprocessor.h
    //! \todo Test all configs, fix bugs, and remove this
    #if PEGASUS_ENGINE
        OutputDebugString("PEGASUS_ENGINE on\n");
    #else
        OutputDebugString("PEGASUS_ENGINE off\n");
    #endif
    
    #if PEGASUS_DEV
        OutputDebugString("PEGASUS_DEV on\n");
    #else
        OutputDebugString("PEGASUS_DEV off\n");
    #endif
    
    #if PEGASUS_REL
        OutputDebugString("PEGASUS_REL on\n");
    #else
        OutputDebugString("PEGASUS_REL off\n");
    #endif
    
    #if PEGASUS_DEBUG
        OutputDebugString("PEGASUS_DEBUG on\n");
    #else
        OutputDebugString("PEGASUS_DEBUG off\n");
    #endif
    
    #if PEGASUS_OPT
        OutputDebugString("PEGASUS_OPT on\n");
    #else
        OutputDebugString("PEGASUS_OPT off\n");
    #endif
    
    #if PEGASUS_FINAL
        OutputDebugString("PEGASUS_FINAL on\n");
    #else
        OutputDebugString("PEGASUS_FINAL off\n");
    #endif
    
    #if PEGASUS_PROFILE
        OutputDebugString("PEGASUS_PROFILE on\n");
    #else
        OutputDebugString("PEGASUS_PROFILE off\n");
    #endif
    
    #if PEGASUS_SMALL
        OutputDebugString("PEGASUS_SMALL on\n");
    #else
        OutputDebugString("PEGASUS_SMALL off\n");
    #endif

    #if PEGASUSAPP_DLL
        OutputDebugString("PEGASUSAPP_DLL on\n");
    #else
        OutputDebugString("PEGASUSAPP_DLL off\n");
    #endif
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{

}

//----------------------------------------------------------------------------------------

void TestApp1::Initialize(const Pegasus::ApplicationConfig& config)
{
    Pegasus::Application::Initialize(config);
}

//----------------------------------------------------------------------------------------

void TestApp1::Shutdown()
{
    Pegasus::Application::Shutdown();
}

//----------------------------------------------------------------------------------------

void TestApp1::Render()
{
    //! \todo Render
}
