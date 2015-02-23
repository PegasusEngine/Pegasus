/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                  */
/*                                                                                      */
/****************************************************************************************/

//! \file   Launcher_Win32.cpp
//! \author Kevin Boulanger, David Worsham
//! \date   02nd June 2013
//! \brief  Entry point of Pegasus Launcher, allowing the execution of application in dev mode
//!         (Win32 and Win64 version)

#include "Pegasus/Preprocessor.h"

#include "Launcher/EventListeners.h"
#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/IWindowRegistry.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/Texture/Shared/ITextureManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"


#if PEGASUS_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>


// Since we know where the launcher launches from, hard-code the asset root
static const char* ASSET_ROOT = ".";


#if PEGASUS_ENABLE_LOG

#include <cstdio>
#include <ctime>

#if PEGASUS_ENABLE_ASSERT
Pegasus::Core::AssertReturnCode AssertionHandler(const char * testStr, const char * fileStr, int line, const char * msgStr);
#endif

//! Maximum size of the buffer containing one log message
static const size_t LOG_BUFFER_SIZE = 2048; 

//! Handler for log messages coming from the application
//! \param logChannel Log channel that receives the message
//! \param msgStr String of the message to log
void LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr)
{
    if (msgStr == nullptr)
    {
#if PEGASUS_ENABLE_ASSERT
        AssertionHandler("msgStr != nullptr", __FILE__, __LINE__, "A log string is undefined.");
#endif
        msgStr = "INVALID";
    }

    // Create the timestamp string
    std::time_t rawTime;
    std::time(&rawTime);
    std::tm timeInfo;
    localtime_s(&timeInfo, &rawTime);
    static char timeString[10] = "";
    std::strftime(timeString, 10, "%H:%M:%S", &timeInfo);

    // Convert the log channel to a string
    static char logChannelString[5] = "";
    logChannelString[0] = static_cast<char>((logChannel >> 24) & 0xFF);
    logChannelString[1] = static_cast<char>((logChannel >> 16) & 0xFF);
    logChannelString[2] = static_cast<char>((logChannel >>  8) & 0xFF);
    logChannelString[3] = static_cast<char>( logChannel        & 0xFF);
    logChannelString[4] = '\0';

    // Build the log string
    static char logBuffer[LOG_BUFFER_SIZE] = "";
    sprintf_s(logBuffer, "%s [%s] %s\n", timeString, logChannelString, msgStr);

    // Output the log string to the console
    OutputDebugString(logBuffer);
}

#endif  // PEGASUS_ENABLE_LOG

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_ASSERT

//! Maximum size of the buffer containing one assertion error message
static const size_t ASSERTIONERROR_BUFFER_SIZE = 2048; 

//! Handler for assertion errors coming from the application
//! \param testStr String representing the assertion test itself
//! \param fileStr String with the filename where the assertion test failed
//! \param line Line number where the assertion test failed
//! \param msgStr Optional string of a message making the assertion test easier to understand.
//!               nullptr if no message is defined.
//! \return Pegasus::Core::ASSERTION_xxx constant,
//!         chosen from the button the user clicked on
Pegasus::Core::AssertReturnCode AssertionHandler(const char * testStr,
                                                 const char * fileStr,
                                                 int line,
                                                 const char * msgStr)
{
    if (testStr == nullptr)
    {
        AssertionHandler("FAILURE", __FILE__, __LINE__, "The test string has to be defined for an assertion error.");
        testStr = "FAILURE";
    }
    if (fileStr == nullptr)
    {
        AssertionHandler("FAILURE", __FILE__, __LINE__, "The file name string has to be defined for an assertion error.");
        fileStr = "UNKNOWN FILE";
    }
    if (line < 0)
    {
        AssertionHandler("line >= 0", __FILE__, __LINE__, "Invalid line number for an assertion error.");
    }

    // If Ignore All has been selected, ignore the current assertion error
    static bool sIgnoreAll = false;
    if (sIgnoreAll)
    {
        return Pegasus::Core::ASSERTION_IGNOREALL;
    }

    // Prepare the content of the dialog box
    static char * titleText = nullptr;
    static char assertionText[ASSERTIONERROR_BUFFER_SIZE] = "";
    if (testStr == "FAILURE")
    {
        titleText = "Failure";
        sprintf_s(assertionText, "File: %s\nLine: %d", fileStr, line);
    }
    else
    {
        titleText = "Assertion error";
        sprintf_s(assertionText, "Test: %s\nFile: %s\nLine: %d", testStr, fileStr, line);
    }

    // Extend the assertion text with the message if defined
    if (msgStr != nullptr)
    {
        strcat_s(assertionText, "\n");
        strcat_s(assertionText, msgStr);
    }

#if PEGASUS_ENABLE_LOG
    //! Send the text of the assertion error to the log
    static char logBuffer[ASSERTIONERROR_BUFFER_SIZE];
    sprintf_s(logBuffer, "%s. %s", titleText, assertionText);
    LogHandler('ASRT', logBuffer);
#endif  // PEGASUS_ENABLE_LOG

    // Extend the assertion text by adding instructions on what button to push
    strcat_s(assertionText, "\n\nClick Abort to break into the debugger.\n"
                                "Click Retry to continue the execution.\n"
                                "Click Ignore to ignore this assertion or all assertions.");

    // Show the dialog box
    const int returnButton = MessageBox(NULL,
                                        assertionText,
                                        titleText,
                                        MB_ABORTRETRYIGNORE | MB_DEFBUTTON2 | MB_ICONERROR | MB_TASKMODAL);

    // Handle the clicked button
    if (returnButton == IDIGNORE)
    {
        const int returnButtonIgnore = MessageBox(NULL,
                                                  "Do you want to ignore all assertion errors?\n\n"
                                                  "Click Yes to ignore every assertion error from now.\n"
                                                  "Clock No to ignore only this specific assertion error from now.",
                                                  "Ignore all assertion errors?",
                                                  MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION | MB_TASKMODAL);
        if (returnButtonIgnore == IDYES)
        {
            sIgnoreAll = true;
            return Pegasus::Core::ASSERTION_IGNOREALL;
        }
        else
        {
            return Pegasus::Core::ASSERTION_IGNORE;
        }
    }
    else if (returnButton == IDABORT)
    {
#if PEGASUS_COMPILER_MSVC
        // Break into the debugger
        __debugbreak();
#else
#error "Debug break is not implemented on this platform"
#endif

        return Pegasus::Core::ASSERTION_BREAK;
    }

    return Pegasus::Core::ASSERTION_CONTINUE;
}

#endif  // PEGASUS_ENABLE_ASSERT

//----------------------------------------------------------------------------------------

//! Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Pegasus::App::ApplicationConfig appConfig;
    Pegasus::App::AppWindowConfig windowConfig;
    Pegasus::App::IApplicationProxy* application = NULL;
    Pegasus::Wnd::IWindowProxy* appWindow = NULL;
    MSG curMsg;
    bool appDone = false;
    int retVal = 0;

    // Check the command line for the name of the app to load.
    // It needs to be at least 5 characters long (x.dll)
    if ((lpCmdLine == NULL) || (strnlen_s(lpCmdLine, 8) < 5))
    {
        MessageBox(NULL,
                   TEXT("Invalid command line for the launcher.\nSyntax: Launcher.exe <AppName>.dll"),
                   TEXT("Invalid command line"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 0;
    }

    // Try to load the DLL
    HMODULE dllModule = LoadLibrary(lpCmdLine);
    if (dllModule == NULL)
    {
        // Get the error code
        const DWORD error = GetLastError();

        // Format the error message for the dialog box
        LPVOID messageBuffer;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&messageBuffer,
                      0,
                      NULL);
        LPTSTR displayBuffer = (LPTSTR)LocalAlloc(LMEM_ZEROINIT,
                                                  (lstrlen((LPCTSTR)messageBuffer) + 64) * sizeof(TCHAR));
        StringCchPrintf(displayBuffer,
                        LocalSize(displayBuffer) / sizeof(TCHAR),
                        TEXT("Unable to load the application DLL:\nError %d: %s"), 
                        error,
                        messageBuffer);
        LocalFree(messageBuffer);

        // Show the error dialog box
        MessageBox(NULL,
                   displayBuffer,
                   TEXT("Application loading"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);

        // Exit
        LocalFree(displayBuffer);
        return error;
    }

    // Retrieve the entry point of the application DLL
    FARPROC createAppProcAddress = GetProcAddress(dllModule, "CreatePegasusApp");
    FARPROC destroyAppProcAddress = GetProcAddress(dllModule, "DestroyPegasusApp");
    if (createAppProcAddress == NULL || destroyAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        MessageBox(NULL,
                   TEXT("The loaded DLL is not a valid Pegasus application."),
                   TEXT("Application loading"),
                   MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 0;
    }

    // Cast the procedure into the actual entry point function
    //! \todo Make the function pointer a declaration in IApplication.h?
    Pegasus::App::CreatePegasusAppFuncPtr CreatePegasusAppFunc = (Pegasus::App::CreatePegasusAppFuncPtr) createAppProcAddress;
    Pegasus::App::DestroyPegasusAppFuncPtr DestroyPegasusAppFunc = (Pegasus::App::DestroyPegasusAppFuncPtr) destroyAppProcAddress;
    
    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Os::ModuleHandle) hInstance;
    appConfig.mMaxWindowTypes = Pegasus::App::NUM_WND_TYPES;
    appConfig.mMaxNumWindows = 1;
    appConfig.mBasePath = ASSET_ROOT;
    // Attach the debugging features
#if PEGASUS_ENABLE_LOG
    appConfig.mLoghandler = LogHandler;
#endif
#if PEGASUS_ENABLE_ASSERT
    appConfig.mAssertHandler = AssertionHandler;
#endif

    // Initialize the application
    application = CreatePegasusAppFunc(appConfig);
    application->Initialize();

    Pegasus::Launcher::LauncherShaderListener  shaderListener(appConfig.mLoghandler, appConfig.mAssertHandler);
    Pegasus::Launcher::LauncherTextureListener textureListener(appConfig.mLoghandler, appConfig.mAssertHandler);
    Pegasus::Launcher::LauncherMeshListener    meshListener(appConfig.mLoghandler, appConfig.mAssertHandler);

    application->GetShaderManagerProxy()->RegisterEventListener(&shaderListener);
    application->GetTextureManagerProxy()->RegisterEventListener(&textureListener);
    application->GetMeshManagerProxy()->RegisterEventListener(&meshListener);


    // Set up window config
    windowConfig.mWindowType = application->GetMainWindowType();
    windowConfig.mIsChild = false;
    windowConfig.mParentWindowHandle = 0;
    windowConfig.mWidth = 960;
    windowConfig.mHeight = 540;

    // Set up windows
    appWindow = application->AttachWindow(windowConfig);
    appWindow->Initialize();

    // Load the assets required to render the timeline blocks
    application->Load();

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
            appWindow->Refresh(true);

            // Dispatch it
            TranslateMessage(&curMsg);
            DispatchMessage(&curMsg);
        }
    }
    retVal = curMsg.wParam;

    // Tear down windows
    appWindow->Shutdown();
    application->DetachWindow(appWindow);

    // Destroy the application
    application->Shutdown();
    DestroyPegasusAppFunc(application);

    // Release the application library
    FreeLibrary(dllModule);

    return retVal;
}

#else
#error "This file is not supposed to be compiled on platforms other than Windows"
#endif  // PEGASUS_PLATFORM_WINDOWS
