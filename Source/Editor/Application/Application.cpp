/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.cpp
//! \author Karolyn Boulanger
//! \date   02nd July 2013
//! \brief  Worker thread to contain an application to run

#include "Application/Application.h"
#include "Application/ApplicationInterface.h"
#include "Application/ApplicationManager.h"

#include "Viewport/ViewportWidget.h"
#include "Pegasus/Core/Shared/OsDefs.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "CodeEditor\SourceCodeManagerEventListener.h"

#include <QTimer>
#include <stdio.h>

#if PEGASUS_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // PEGASUS_PLATFORM_WINDOWS


Application::Application(QObject *parent)
:   QThread(parent),
    mApplicationInterface(nullptr),
    mFileName(),
    mApplication(nullptr),
    mAsyncHandleDestruction(true),
    mAssertionReturnCode(AssertionManager::ASSERTION_INVALID)
{
    // Make the application thread priority lower than the UI, so the UI is way more reactive
    setPriority(QThread::LowPriority);
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
}

//----------------------------------------------------------------------------------------

void Application::SetFileName(const QString & fileName)
{
    ED_ASSERTSTR(!fileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    mFileName = fileName;
}

//----------------------------------------------------------------------------------------

void Application::run()
{
    ED_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string.");
    Pegasus::App::ApplicationConfig appConfig;
    int retVal = 0;

    //PG_ASSERTSTR(!mFileName.isEmpty(), "Invalid application to open, the name cannot be an empty string");
    if (mFileName.isEmpty())
    {
        emit(LoadingError(ERROR_INVALID_FILE_NAME));
        return;
    }

#if PEGASUS_PLATFORM_WINDOWS
    int lbrac = mFileName.lastIndexOf('/');
    int rbrac = mFileName.lastIndexOf('\\');
    int lastIndex = lbrac > rbrac ? lbrac : rbrac;    
    QString assetRoot = mFileName.mid(0, lastIndex);
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

    // Cast the procedure into the actual entry point function
    Pegasus::App::CreatePegasusAppFuncPtr CreatePegasusAppFunc = (Pegasus::App::CreatePegasusAppFuncPtr) createAppProcAddress;
    Pegasus::App::DestroyPegasusAppFuncPtr DestroyPegasusAppFunc = (Pegasus::App::DestroyPegasusAppFuncPtr) destroyAppProcAddress;

    // Set up app config
    appConfig.mModuleHandle = (Pegasus::Os::ModuleHandle) GetModuleHandle(NULL); // Use the handle of the Editor EXE
    QByteArray byteArr = assetRoot.toLocal8Bit();
    appConfig.mBasePath = byteArr.data();
    // Attach the debugging features
    // (queued connections as the connections are between threads)
    ApplicationManager * applicationManager = qobject_cast<ApplicationManager *>(parent());
    if (applicationManager != nullptr)
    {
        qRegisterMetaType<Pegasus::Core::LogChannel>("Pegasus::Core::LogChannel");
	    connect(this, SIGNAL(LogSentFromApplication(Pegasus::Core::LogChannel, const QString &)),
                this, SLOT(LogReceivedFromApplication(Pegasus::Core::LogChannel, const QString &)),
                Qt::QueuedConnection);
        appConfig.mLoghandler = LogHandler;

        connect(this, SIGNAL(AssertionSentFromApplication(const QString &, const QString &, int, const QString &)),
                this, SLOT(AssertionReceivedFromApplication(const QString &, const QString &, int, const QString &)),
                Qt::QueuedConnection);
        appConfig.mAssertHandler = AssertionHandler;
    }
    else
    {
        ED_FAILSTR("Unable to register the assertion handler, since Application object's parent is not an ApplicationManager.");
    }

    // Initialize the application
    mApplication = CreatePegasusAppFunc(appConfig);

    // Create the application interface object, owned by the application thread.
    // Not a child of this QThread, since we want it to be in the application thread.
    mApplicationInterface = new ApplicationInterface(this);

    // Load the assets required to render the timeline blocks
    mApplication->Load();

    // Signal the success of the loading
    emit(LoadingSucceeded());

    // Run the application loop. Does not use Application->Run() since we want to control
    // the sequencing of the message loop from the editor, and to allow assertion dialog boxes to work correctly.
    // Uses QThread::exec() rather than QEventLoop::exec() to allow timers to work.
    this->exec();

    mApplication->Unload();
    mApplicationInterface->DestroyAllWindows();

    // Destroy the application
    DestroyPegasusAppFunc(mApplication);

    // Kill the interface with the application
    delete mApplicationInterface;
    mApplicationInterface = nullptr;


    
#if PEGASUS_PLATFORM_WINDOWS

    // Release the application library
    FreeLibrary(dllModule);

#else
#error "Implement the unloading of the application library"
#endif  // PEGASUS_PLATFORM_WINDOWS
    if (mAsyncHandleDestruction)
    {
        emit(ApplicationFinished());
    }
}

//----------------------------------------------------------------------------------------

Pegasus::PropertyGrid::IPropertyGridManagerProxy * Application::GetPropertyGridManagerProxy() const
{
    if (mApplication != nullptr)
    {
        return mApplication->GetPropertyGridManagerProxy();
    }
    else
    {
        ED_FAILSTR("Invalid Pegasus application object when getting the property grid manager proxy");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Timeline::ITimelineManagerProxy * Application::GetTimelineProxy() const
{
    if (mApplication != nullptr)
    {
        return mApplication->GetTimelineManagerProxy();
    }
    else
    {
        ED_FAILSTR("Invalid Pegasus application object when getting the timeline proxy");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Shader::IShaderManagerProxy * Application::GetShaderManagerProxy() const
{
    if (mApplication != nullptr)
    {
        return mApplication->GetShaderManagerProxy();
    }
    else
    {
        ED_FAILSTR("Invalid Pegasus application object when getting the shader manager proxy");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Texture::ITextureManagerProxy * Application::GetTextureManagerProxy() const
{
    if (mApplication != nullptr)
    {
        return mApplication->GetTextureManagerProxy();
    }
    else
    {
        ED_FAILSTR("Invalid Pegasus application object when getting the texture manager proxy");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Application::EmitLogFromApplication(Pegasus::Core::LogChannel logChannel, const QString & msgStr)
{
    // Emit the log message through an enqueued connection to the editor thread
    emit LogSentFromApplication(logChannel, msgStr);
}

//----------------------------------------------------------------------------------------

Pegasus::Core::AssertReturnCode Application::EmitAssertionFromApplication(const QString & testStr,
                                                                          const QString & fileStr,
                                                                          int line,
                                                                          const QString & msgStr)
{
    if (mApplicationInterface != nullptr)
    {
        // Stop the forced redraw of the window content
        //! \todo Make sure the play mode is blocked when throwing an assertion

        // Tell the app windows to not render anything
        //! \todo Seems not useful anymore. Test and remove if possible
        //mApplicationInterface->SetAssertionBeingHandled(true);
    }

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

    Pegasus::Core::AssertReturnCode returnCode = Pegasus::Core::ASSERTION_CONTINUE;
    if (mAssertionReturnCode == AssertionManager::ASSERTION_IGNORE)
    {
        returnCode = Pegasus::Core::ASSERTION_IGNORE;
    }
    else if (mAssertionReturnCode == AssertionManager::ASSERTION_IGNOREALL)
    {
        returnCode = Pegasus::Core::ASSERTION_IGNOREALL;
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

        returnCode = Pegasus::Core::ASSERTION_BREAK;
    }

    // Reset the return code until the next assertion error   
    mAssertionReturnCode = AssertionManager::ASSERTION_INVALID;

    if (mApplicationInterface != nullptr)
    {
        // Allow the app windows to render their content 
        //! \todo Seems not useful anymore. Test and remove if possible
        //mApplicationInterface->SetAssertionBeingHandled(false);

        //! \todo Make sure play mode still works after an assertion error
    }

    return returnCode;
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

void Application::UpdateUIAndRequestFrameInPlayMode(float beat)
{
    // Update the timeline UI (beat, cursor)
    Editor::GetInstance().GetTimelineDockWidget()->UpdateUIFromBeat(beat);

    // If the play mode is still enabled, send a new request to refresh the viewport
    if (Editor::GetInstance().GetTimelineDockWidget()->IsPlaying())
    {
        emit FrameRequestedInPlayMode();
    }
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

Pegasus::Core::AssertReturnCode Application::AssertionHandler(const char * testStr,
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
        return Pegasus::Core::ASSERTION_BREAK;
    }
}
