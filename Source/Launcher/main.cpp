#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>

#include "Pegasus/TestPegasus.h"

//! Message pump handler
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
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
    default:
        break;
    }

    // Fall back to OS default
    return (DefWindowProc(hwnd, message, wParam, lParam));
}


//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX windowClass;
    HWND windowHandle;
    MSG curMsg;
    bool appDone = false;

    // Set up our window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
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
            // Dispatch it
            TranslateMessage(&curMsg); 
            DispatchMessage(&curMsg);
        }
    }

    // wParam of WM_QUIT is the app exit code
    return curMsg.wParam;
}
