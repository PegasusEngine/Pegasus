/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Main.cpp
//! \author	Kevin Boulanger, David Worsham
//! \date	02nd June 2013
//! \brief	Entry point of Pegasus Launcher, allowing the execution of application in dev mode

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <gl/gl.h>

#include "Pegasus/Pegasus.h"
#include "Pegasus/TestPegasus.h"


static unsigned int gFrameCount = 1; // Frame counter
static float gClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f}; // Clear color
static HDC gHDC; //! global device context
static HGLRC gRC; //! global render context
static PIXELFORMATDESCRIPTOR gPixelFormat = {
    sizeof(PIXELFORMATDESCRIPTOR), //! size of structure
    1, //! default version
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //! flags
    PFD_TYPE_RGBA, //! RGBA color mode
    32, //! 32 bit color mode
    0, 0, 0, 0, 0, 0, //! ignore color bits
    0, //! no alpha buffer
    0, //! ignore shift bit
    0, //! no accumulation buffer
    0, 0, 0, 0, //! ignore accumulation bits
    24, //! 24 bit z-buffer size
    8, //! 8 bit stencil-buffer size
    0, //! no aux buffer
    PFD_MAIN_PLANE, //! main drawing plane
    0, //! reserved
    0, 0, 0}; //! layer masks ignored


//! Message pump handler
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE: // Window is being created
        {
            // Cache global context
            gHDC = GetDC(hwnd);

            // Setup pixel format for backbuffer
            int nPixelFormat = ChoosePixelFormat(gHDC, &gPixelFormat);
            SetPixelFormat(gHDC, nPixelFormat, &gPixelFormat);

            // Make a new opengl context and link it to the window
            gRC = wglCreateContext(gHDC);
            wglMakeCurrent(gHDC, gRC);
        }
        return 0;
    case WM_DESTROY: // Window is being destroyed
        {
            wglMakeCurrent(gHDC, NULL);
            wglDeleteContext(gRC);
        }
        return 0;
    case WM_CLOSE: // Someone asked to close the window
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        /*
        // Draw test string in blue text
        {
            PAINTSTRUCT paintStruct;
            HDC hDC;
            char printBuffer[128];

            // Compute string
            sprintf(printBuffer, "Test of %i is %i!\n", 3, testFunc(3));

            // Draw
            hDC = BeginPaint(hwnd, &paintStruct);
            SetTextColor(hDC, COLORREF(0x00FF0000));
            TextOut(hDC, 150, 150, printBuffer, strlen(printBuffer));
            EndPaint(hwnd, &paintStruct);
        }
        return 0;
        */
    default:
        break;
    }

    // Fall back to OS default
    return (DefWindowProc(hwnd, message, wParam, lParam));
}


void Render()
{
    // Update clear color once per second
    if (gFrameCount % 60 == 0)
    {
        gClearColor[0] = (((float) rand()) / ((float) RAND_MAX));
        gClearColor[1] = (((float) rand()) / ((float) RAND_MAX));
        gClearColor[2] = (((float) rand()) / ((float) RAND_MAX));
    }
    gFrameCount++;

    // Clear screen
    glClearColor(gClearColor[0], gClearColor[1], gClearColor[2], gClearColor[3]);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Present
    glFlush();
    SwapBuffers(gHDC);
}


//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    // Test for Preprocessor.h
    // Can be removed when not required anymore.
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


    WNDCLASSEX windowClass;
    HWND windowHandle;
    MSG curMsg;
    bool appDone = false;

    // Seed RNG
    srand((unsigned int) time(NULL));

    // Set up our window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc; // Message pump callback
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "PegLaunch";
    windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register our window class with the OS
    if (!RegisterClassEx(&windowClass))
    {
        MessageBox(NULL, "Failed to create window class!", "Fatal Error", MB_OK);
        return 1;
    }

    // Create window
    windowHandle = CreateWindowEx(NULL,
                                  "PegLaunch",
                                  "Pegasus Test Launcher",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
                                  100, 100,
                                  960, 540,
                                  NULL,
                                  NULL,
                                  hInstance,
                                  NULL);
    if (!windowHandle)
    {
        MessageBox(NULL, "Failed to create window!", "Fatal Error", MB_OK);
        return 1;
    }

    // Run message pump until application exits
    while(!appDone)
    {
        // Grab a message and dispatch it
        PeekMessage(&curMsg, NULL, NULL, NULL, PM_REMOVE);
        if (curMsg.message == WM_QUIT)
        {
            // Bail out, app is finished
            appDone = true;
        }
        else
        {
            // Draw stuff
            Render();

            // Dispatch it
            TranslateMessage(&curMsg); 
            DispatchMessage(&curMsg);
        }
    }

    // wParam of WM_QUIT is the app exit code
    return curMsg.wParam;
}
