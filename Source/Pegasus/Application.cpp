/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Application.cpp
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#include "Pegasus/Application.h"
#include "Pegasus/Window.h"
#include "Pegasus/Render/GL/GLExtensions.h"
#include <windows.h>


namespace Pegasus {

// bootstrapped flag
bool Application::sContextBootstrapped = false;
unsigned int Application::sNumInstances = 0;


//! Basic constructor.
ApplicationConfig::ApplicationConfig()
    : mAppHandle(NULL)
{
}

//----------------------------------------------------------------------------------------

//! Config-based constructor.
//! \param apphandle Application handle to config with.
ApplicationConfig::ApplicationConfig(ApplicationHandle apphandle)
    : mAppHandle(apphandle)
{
}

//----------------------------------------------------------------------------------------

//! Basic destructor.
ApplicationConfig::~ApplicationConfig()
{
}

//----------------------------------------------------------------------------------------

//! Basic constructor.
Application::Application()
    : mInitialized(false), mAppHandle(NULL), mNumWindows(0)
{
    // Init windows array
    for (unsigned int i = 0; i < MAX_NUM_WINDOWS; i++)
    {
        mWindows[i] = nullptr;
    }
}

//----------------------------------------------------------------------------------------

//! Destructor.
Application::~Application()
{
    // shutdown the extensions manager if need be
    sNumInstances--;
    if (sNumInstances == 0 && sContextBootstrapped)
    {
        ShutdownAppInternal();
        sContextBootstrapped = false;
    }

    //! \todo Assert that no windows exist here, to alert the user of them being stupid

    // Free windows
    for (unsigned int i = 0; i < MAX_NUM_WINDOWS; i++)
    {
        if (mWindows[i] != nullptr)
        {
            DetachWindow(mWindows[i]);
        }
    }
}

//----------------------------------------------------------------------------------------

//! Creates an application window and links it to this app.
//! \param config Config object used to create the window.
//! \return The created window, or nullptr if creation failed.
Window* Application::AttachWindow(const WindowConfig& config)
{
    Window* newWnd = nullptr;
    Window::WindowConfigPrivate privConfig(config);

    // Create window
    //! \todo Assert/log on failure
    privConfig.mAppHandle = mAppHandle;
    privConfig.mIsStartupWindow = false;
    newWnd = new Window(privConfig);

    // Assign it
    //! \todo Assert on windows list not full
    if (newWnd)
    {
        mWindows[mNumWindows++] = newWnd;
    }

    return newWnd;
}

//----------------------------------------------------------------------------------------

//! Destroys a window and unlinks it from this app.
//! \param wnd The window to destroy.
void Application::DetachWindow(Window* wnd)
{
    //! \todo Assert window not null
    //! \todo Assert window ID in range
    delete mWindows[mNumWindows - 1];
    mWindows[mNumWindows - 1] = nullptr;
    mNumWindows--;
}

//----------------------------------------------------------------------------------------

//! Runs the application loop for this Pegasus app.
//! \note This method does not return until the user closes the application.
//! \return Status code.
int Application::Run()
{
    MSG curMsg;
    bool appDone = false;

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

//----------------------------------------------------------------------------------------

//! Initialize this application.
//! \param config Config structure to create this app with.
//! \warning All other method calls are invalid until Initialize is called.
void Application::Initialize(const ApplicationConfig& config)
{
    Window* startupWindow = nullptr;

    // Cache off application handle
    mAppHandle = config.mAppHandle;

    //! \todo Refactor this instance counting junk
    // start up the app, which creates and destroys the dummy window
    if (!sContextBootstrapped)
    {
        StartupAppInternal();
        sContextBootstrapped = true;
    }
    sNumInstances++;

    //! \todo Assert on init
    // Initted
    mInitialized = true;
}

//----------------------------------------------------------------------------------------

//! Shutdown this application.
void Application::Shutdown()
{
    //! \todo Assert on init
    // No longer initted
    mInitialized = false;
}

//----------------------------------------------------------------------------------------

//! Starts up the application.
//! \note Creates the dummy startup window used to initialize the OGL extensions.
void Application::StartupAppInternal()
{
    Window* newWnd = nullptr;
    WindowConfig baseConfig;
    Window::WindowConfigPrivate privConfig(baseConfig);

    // Create window and immediately destroy it
    //! \todo Assert/log on failure
    privConfig.mAppHandle = mAppHandle;
    privConfig.mIsStartupWindow = true;
    newWnd = new Window(privConfig);

    // Init openGL extensions now that we have a context
    Render::GLExtensions::CreateInstance();

    // Write some temporary debugging information
    //! \todo Remove this after testing
    Render::GLExtensions & extensions = Render::GLExtensions::GetInstance();
    switch (extensions.GetMaximumProfile())
    {
    case Render::GLExtensions::PROFILE_GL_3_3:
        OutputDebugString("OpenGL 3.3 is the maximum detected profile.\n");
        break;

    case Render::GLExtensions::PROFILE_GL_4_3:
        OutputDebugString("OpenGL 4.3 is the maximum detected profile.\n");
        break;

    default:
        OutputDebugString("Error when initializing GLExtensions.\n");
        break;
    }
    if (extensions.IsGLExtensionSupported("GL_ARB_draw_indirect"))
    {
        OutputDebugString("GL_ARB_draw_indirect detected.\n");
    }
    else
    {
        OutputDebugString("GL_ARB_draw_indirect NOT detected.\n");
    }
    if (extensions.IsGLExtensionSupported("GL_ATI_fragment_shader"))
    {
        OutputDebugString("GL_ATI_fragment_shader detected.\n");
    }
    else
    {
        OutputDebugString("GL_ATI_fragment_shader NOT detected.\n");
    }
    if (extensions.IsWGLExtensionSupported("WGL_ARB_buffer_region"))
    {
        OutputDebugString("WGL_ARB_buffer_region detected.\n");
    }
    else
    {
        OutputDebugString("WGL_ARB_buffer_region NOT detected.\n");
    }
    if (extensions.IsWGLExtensionSupported("WGL_3DL_stereo_control"))
    {
        OutputDebugString("WGL_3DL_stereo_control detected.\n");
    }
    else
    {
        OutputDebugString("WGL_3DL_stereo_control NOT detected.\n");
    }

    // Destroy the window, it is no longer needed
    delete newWnd;
}

//----------------------------------------------------------------------------------------

//! shuts down the application.
void Application::ShutdownAppInternal()
{
    // Destroy openGL extensions
    Render::GLExtensions::DestroyInstance();
}

//----------------------------------------------------------------------------------------


}   // namespace Pegasus
