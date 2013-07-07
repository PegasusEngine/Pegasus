/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                   */
/*                                                                                      */
/****************************************************************************************/

//! \file   main.cpp
//! \author Kevin Boulanger, David Worsham
//! \date   02nd June 2013
//! \brief  Entry point of Pegasus Launcher, allowing the execution of application in dev mode

#define WIN32_LEAN_AND_MEAN

#include "Pegasus/Pegasus.h"
#include <windows.h>


//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
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
    
    #if PEGASUS_DLL
        OutputDebugString("PEGASUS_DLL on\n");
    #else
        OutputDebugString("PEGASUS_DLL off\n");
    #endif

/*    Pegasus::ApplicationConfig testAppConfig((Pegasus::Core::PG_HINSTANCE) hInstance);
    Pegasus::Application testApp(testAppConfig);
    Pegasus::Core::WindowConfig testWindowConfig;
    Pegasus::Core::Window* testWindow = testApp.AttachWindow(testWindowConfig);
    int retVal = 0;

    // Run app
    retVal = testApp.Run();
    testApp.DetachWindow(testWindow);

    return retVal;*/
    return 0;
}

// unsigned int gFrameCount = 0;
// float gClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
//void Render()
//{
//    // Update clear color once per second
//    if (gFrameCount % 60 == 0)
//    {
//        gClearColor[0] = (((float) rand()) / ((float) RAND_MAX));
//        gClearColor[1] = (((float) rand()) / ((float) RAND_MAX));
//        gClearColor[2] = (((float) rand()) / ((float) RAND_MAX));
//    }
//    gFrameCount++;
//
//    // Clear screen
//    glClearColor(gClearColor[0], gClearColor[1], gClearColor[2], gClearColor[3]);
//    glClearDepth(0.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // Present
//    glFlush();
//    SwapBuffers(gHDC);
//}
