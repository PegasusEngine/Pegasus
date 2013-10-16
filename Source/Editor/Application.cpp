/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Application.cpp
//! \author	Kevin Boulanger
//! \date	02nd July 2013
//! \brief	Worker thread to contain an application to run

#include "Application.h"
#include "ApplicationManager.h"

#include "Viewport/ViewportWidget.h"
#include "Pegasus/Preprocessor.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"

#include <QTimer>

#include <stdio.h>

#if PEGASUS_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // PEGASUS_PLATFORM_WINDOWS


Application::Application(QObject *parent)
:   QThread(parent),
    mFileName(),
    mApplication(nullptr),
    mAppWindow(nullptr),
    mViewportWindowHandle(0),
    mViewportInitialWidth(128),
    mViewportInitialHeight(128),
    mAssertionBeingHandled(false),
    mAssertionReturnCode(AssertionManager::ASSERTION_INVALID)
{
    // Create the window redrawing timer
    // (5 ms to avoid spamming the message loop when user interaction happens, but low enough to get a good framerate)
    mTimer = new QTimer(nullptr);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(RedrawChildWindows()));
    mTimer->setInterval(5);

    // Move the timer to the application thread, as we want it to be triggered only once per message loop processing
    mTimer->moveToThread(this);
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
}

//----------------------------------------------------------------------------------------

void Application::SetFile(const QString & fileName)
{
    ED_ASSERTSTR(!fileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    mFileName = fileName;
}

//----------------------------------------------------------------------------------------

void Application::SetViewportParameters(/**index,*/ Pegasus::Window::WindowHandle windowHandle,
                                        int width, int height)
{
    ED_ASSERTSTR(windowHandle != 0, "Invalid viewport window handle set for an application.");
    ED_ASSERTSTR(width > 0, "Invalid viewport initial width (%d). It must be positive.", width);
    ED_ASSERTSTR(height > 0, "Invalid viewport initial height (%d). It must be positive.", height);

    mViewportWindowHandle = windowHandle;
    mViewportInitialWidth = width;
    mViewportInitialHeight = height;
}

//----------------------------------------------------------------------------------------

void Application::run()
{
    ED_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    Pegasus::Application::ApplicationConfig appConfig;
    Pegasus::Application::AppWindowConfig windowConfig;
    int retVal = 0;

    //PG_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    if (mFileName.isEmpty())
    {
        emit(LoadingError(ERROR_INVALID_FILE_NAME));
        return;
    }

    if (mViewportWindowHandle == 0)
    {
        ED_FAILSTR("Invalid viewport window handle set for an application.");
        emit(LoadingError(ERROR_INVALID_VIEWPORT));
        return;
    }

#if PEGASUS_PLATFORM_WINDOWS

    // Load the DLL
    HMODULE dllModule = LoadLibrary(mFileName.utf16());
    if (dllModule == NULL)
    {
        emit(LoadingError(ERROR_INVALID_APPLICATION));
        return;
    }

    // Retrieve the entry point of the application DLL
    FARPROC createAppProcAddress = GetProcAddress(dllModule, "CreatePegasusApp");
    if (createAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        emit(LoadingError(ERROR_INVALID_INTERFACE));
        return;
    }
    FARPROC destroyAppProcAddress = GetProcAddress(dllModule, "DestroyPegasusApp");
    if (destroyAppProcAddress == NULL)
    {
        FreeLibrary(dllModule);
        emit(LoadingError(ERROR_INVALID_INTERFACE));
        return;
    }

#else
#error "Implement the loading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS

    // Set up the app config
    appConfig.mModuleHandle = (Pegasus::Window::ModuleHandle) GetModuleHandle(NULL); // Use the handle of the Editor EXE

    // Cast the procedure into the actual entry point function
    Pegasus::Application::CreatePegasusAppFuncPtr CreatePegasusAppFunc = (Pegasus::Application::CreatePegasusAppFuncPtr) createAppProcAddress;
    Pegasus::Application::DestroyPegasusAppFuncPtr DestroyPegasusAppFunc = (Pegasus::Application::DestroyPegasusAppFuncPtr) destroyAppProcAddress;

    // Initialize the application
    mApplication = CreatePegasusAppFunc();
    mApplication->Initialize(appConfig);

    // Attach the debugging features
    // (queued connections as the connections are between threads)
    ApplicationManager * applicationManager = qobject_cast<ApplicationManager *>(parent());
    if (applicationManager != nullptr)
    {
        qRegisterMetaType<Pegasus::Core::LogChannel>("Pegasus::Core::LogChannel");
	    connect(this, SIGNAL(LogSentFromApplication(Pegasus::Core::LogChannel, const QString &)),
                this, SLOT(LogReceivedFromApplication(Pegasus::Core::LogChannel, const QString &)),
                Qt::QueuedConnection);
        mApplication->RegisterLogHandler(LogHandler);

        connect(this, SIGNAL(AssertionSentFromApplication(const QString &, const QString &, int, const QString &)),
                this, SLOT(AssertionReceivedFromApplication(const QString &, const QString &, int, const QString &)),
                Qt::QueuedConnection);
        mApplication->RegisterAssertionHandler(AssertionHandler);
    }
    else
    {
        ED_FAILSTR("Unable to register the assertion handler, since Application object's parent is not an ApplicationManager.");
    }

    //! Set the window handler parent of the created child window
    windowConfig.mIsChild = true;
    windowConfig.mParentWindowHandle = mViewportWindowHandle;
    windowConfig.mWidth = mViewportInitialWidth;
    windowConfig.mHeight = mViewportInitialHeight;

    // Signal the success of the loading
    emit(LoadingSucceeded());

    // Set up windows
    mAppWindow = mApplication->AttachWindow(windowConfig);

    // Start the timer that forces the redrawing of the app windows
    mTimer->start();

    // Run the application loop. Does not use Application->Run() since we want to control
    // the sequencing of the message loop from the editor, and to allow assertion dialog boxes to work correctly.
    // Uses QThread::exec() rather than QEventLoop::exec() to allow the mTimer to work.
    this->exec();

    // Stop the redrawing timer
    mTimer->stop();

    // Tear down windows
    mApplication->DetachWindow(mAppWindow);

    // Destroy the application
    mApplication->Shutdown();
    DestroyPegasusAppFunc(mApplication);

#if PEGASUS_PLATFORM_WINDOWS

    // Release the application library
    FreeLibrary(dllModule);

#else
#error "Implement the unloading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS
}

//----------------------------------------------------------------------------------------

void Application::EmitLogFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr)
{
    // Emit the log message through an enqueued connection to the editor thread
    emit LogSentFromApplication(logChannel, msgStr);
}

//----------------------------------------------------------------------------------------

Pegasus::Core::AssertionManager::ReturnCode Application::EmitAssertionFromApplication(const QString & testStr,
                                                                                      const QString & fileStr,
                                                                                      int line,
                                                                                      const QString & msgStr)
{
    // Stop the forced redraw of the window content
    mTimer->stop();

    // Tell the app windows to not render anything
    mAssertionBeingHandled = true;

    // Emit the assertion error through a non-blocking enqueued connection to the editor thread
    emit AssertionSentFromApplication(testStr, fileStr, line, msgStr);

    // Run an event loop, so we can intercept Qt and OS messages.
    // Stop the loop once we have an available assertion return code.
    mAssertionReturnCode = AssertionManager::ASSERTION_INVALID;
    QEventLoop * eventLoop = new QEventLoop(nullptr);
    while (mAssertionReturnCode == AssertionManager::ASSERTION_INVALID)
    {
        eventLoop->processEvents(QEventLoop::AllEvents);
    }
    delete eventLoop;

    Pegasus::Core::AssertionManager::ReturnCode returnCode = Pegasus::Core::AssertionManager::ASSERTION_CONTINUE;
    if (mAssertionReturnCode == AssertionManager::ASSERTION_IGNORE)
    {
        returnCode = Pegasus::Core::AssertionManager::ASSERTION_IGNORE;
    }
    else if (mAssertionReturnCode == AssertionManager::ASSERTION_IGNOREALL)
    {
        returnCode = Pegasus::Core::AssertionManager::ASSERTION_IGNOREALL;
    }
    else if (mAssertionReturnCode == AssertionManager::ASSERTION_BREAK)
    {
        // If a debug break is required, perform it here to get the debugger on the right thread and close to the assertion function call
#if PEGASUS_COMPILER_MSVC
        // Break into the debugger
        __debugbreak();
#else
#error "Debug break is not implemented on this platform"
#endif

        returnCode = Pegasus::Core::AssertionManager::ASSERTION_BREAK;
    }

    // Reset the return code until the next assertion error   
    mAssertionReturnCode = AssertionManager::ASSERTION_INVALID;

    // Allow the app windows to render their content 
    mAssertionBeingHandled = false;

    // Restart the forceful redrawing of the app windows
    mTimer->start();

    return returnCode;
}

//----------------------------------------------------------------------------------------

void Application::ViewportResized(int width, int height)
{
    ED_ASSERT(mApplication != nullptr);
    ED_ASSERT(mAppWindow != nullptr);

    //! \todo Handle multiple windows
    mApplication->ResizeWindow(mAppWindow, width, height);
}

//----------------------------------------------------------------------------------------

void Application::RedrawChildWindows()
{
    // Let the redrawing happen only when no assertion dialog is present
    if (!mAssertionBeingHandled)
    {
        mApplication->InvalidateWindows();
    }
}

//----------------------------------------------------------------------------------------

void Application::LogReceivedFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr)
{
    Editor::GetInstance().GetLogManager().LogNoFormat(logChannel, msgStr);
}

//----------------------------------------------------------------------------------------

void Application::AssertionReceivedFromApplication(const QString & testStr, const QString & fileStr, int line, const QString & msgStr)
{
    // Open the actual assertion dialog box.
    // Store the return code to unfreeze the application thread.
    mAssertionReturnCode = Editor::GetInstance().GetAssertionManager().AssertionErrorNoFormat(testStr, fileStr, line, msgStr, false);
}

//----------------------------------------------------------------------------------------

void Application::LogHandler(Pegasus::Core::LogChannel logChannel, const char * msgStr)
{
    // Static log handler, so it cannot emit any signal.
    // We have to call a member function, running in the application thread
    Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        // Convert msgStr to a string, then send it through an enqueued connection
        // to the editor thread
        application->EmitLogFromApplication(logChannel, msgStr);
    }
    else
    {
        ED_FAILSTR("Trying to log a message from the current application, which is undefined.");
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Core::AssertionManager::ReturnCode Application::AssertionHandler(const char * testStr,
                                                                          const char * fileStr,
                                                                          int line,
                                                                          const char * msgStr)
{
    // Static assertion handler, so it cannot emit any signal.
    // We have to call a member function, running in the application thread
    Application * const application = Editor::GetInstance().GetApplicationManager().GetApplication();
    if (application != nullptr)
    {
        // Convert testStr, fileStr and msgStr to strings, then send them through a blocking enqueued connection to the editor thread
        return application->EmitAssertionFromApplication(testStr, fileStr, line, msgStr);
    }
    else
    {
        ED_FAILSTR("Trying to send an assertion error from the current application, which is undefined.");
        return Pegasus::Core::AssertionManager::ASSERTION_BREAK;
    }
}
