/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Launcher_Win32.cpp
//! \author Kevin Boulanger
//! \date   08th July 2013
//! \brief  Engine entry point, defined only in release mode (Windows implementation)

#if PEGASUS_INCLUDE_LAUNCHER
#if PEGASUS_PLATFORM_WINDOWS
#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Since this is REL mode only, hard-code the asset root
static const char* ASSET_ROOT = "..\\..\\..\\..\\Data\\";

// Typedefs for DLL entry point
extern Pegasus::App::Application* CreateApplication(const Pegasus::App::ApplicationConfig& config);
extern void DestroyApplication(Pegasus::App::Application* app);

//----------------------------------------------------------------------------------------

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
//! \return Pegasus::Core::AssertionManager::ASSERTION_xxx constant,
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

// App entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    Pegasus::App::ApplicationConfig appConfig;
    Pegasus::App::AppWindowConfig windowConfig;
    Pegasus::App::Application* application = NULL;
    Pegasus::Wnd::Window* appWindow = NULL;
    MSG curMsg;
    bool appDone = false;
    int retVal = 0;

    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Os::ModuleHandle) hInstance;
    appConfig.mMaxWindowTypes = 1;
    appConfig.mMaxNumWindows = 1;
    appConfig.mBasePath = ASSET_ROOT;
#if PEGASUS_ENABLE_LOG
    appConfig.mLoghandler = LogHandler; // Attach the debugging features
#endif
#if PEGASUS_ENABLE_ASSERT
    appConfig.mAssertHandler = AssertionHandler; // Attach the debugging features
#endif

    // Initialize the application
    application = CreateApplication(appConfig);
    application->Initialize();

    // Set up window config
    windowConfig.mWindowType = application->GetWindowRegistry()->GetMainWindowType();
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
    DestroyApplication(application);

    return retVal;
}

#endif  // PEGASUS_PLATFORM_WINDOWS
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_INCLUDE_LAUNCHER
