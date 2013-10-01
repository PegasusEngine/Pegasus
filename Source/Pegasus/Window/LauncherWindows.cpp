/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LauncherWindows.cpp
//! \author Kevin Boulanger
//! \date   08th July 2013
//! \brief  Engine entry point, defined only in release mode (Windows implementation)

#ifndef PEGASUS_LAUNCHERWINDOWS_H
#define PEGASUS_LAUNCHERWINDOWS_H

#include "Pegasus/Preprocessor.h"

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_INCLUDE_LAUNCHER
#if PEGASUS_PLATFORM_WINDOWS

#include "Pegasus/Application.h"

extern Pegasus::Application * CreatePegasusApp();

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <strsafe.h>


//! Engine entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    //! \todo Implement the loading and the main loop of the application

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

    // Call the application global function to create the application object
    Pegasus::Application * application = CreatePegasusApp();

    //! \todo Implement the initialization and the main loop

    // Destroy the application object
    delete application;

    return 0;
}


#else
#error "This file is not supposed to be compiled on platforms other than Windows"
#endif  // PEGASUS_PLATFORM_WINDOWS

#endif  // PEGASUS_INCLUDE_LAUNCHER

#endif  // PEGASUS_LAUNCHERWINDOWS_H
