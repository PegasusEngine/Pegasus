/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusApp.cpp
//! \author	David Worsham
//! \date	4th July 2013
//! \brief	Building block class for a Pegasus application.
//!         Manages access to the Pegasus runtime.

#include "Pegasus/PegasusApp.h"
#include "Pegasus/Core/Window/PegasusWindow.h"
#include "Pegasus/Render/GL/GLExtensions.h"
#include <windows.h>


namespace Pegasus {

// bootstrapped flag
bool Application::sContextBootstrapped = false;
unsigned int Application::sNumInstances = 0;

//! Basic constructor.
//! \param config Config structure to create this app with.
Application::Application(const ApplicationConfig& config)
    : mHINSTANCE(config.mHINSTANCE), mNumWindows(0)
{
    Core::Window* startupWindow = nullptr;

    // Init windows array
    for (unsigned int i = 0; i < MAX_NUM_WINDOWS; i++)
    {
        mWindows[i] = nullptr;
    }

    // start up the app, which creates and destroys the dummy window
    if (!sContextBootstrapped)
    {
        StartupAppInternal();
        sContextBootstrapped = true;
    }
    sNumInstances++;
}

//----------------------------------------------------------------------------------------

//! Basic destructor.
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
Core::Window* Application::AttachWindow(const Core::WindowConfig& config)
{
    Core::Window* newWnd = nullptr;
    Core::Window::WindowConfigPrivate privConfig(config);

    // Create window
    //! \todo Assert/log on failure
    privConfig.mHINSTANCE = mHINSTANCE;
    privConfig.mIsStartupWindow = false;
    newWnd = new Core::Window(privConfig);

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
void Application::DetachWindow(Core::Window* wnd)
{
    //! \todo Assert window not null
    //! \todo Assert window ID in range
    delete mWindows[mNumWindows - 1];
    mWindows[mNumWindows - 1] = nullptr;
    mNumWindows--;
}

//----------------------------------------------------------------------------------------

//! Runs the application loop for this pegasus app.
//! \note This method does not return until the user closes the application.
//! \return Status code.
int Application::Run()
{
    MSG curMsg;
    bool appDone = false;

    // Init app
    Initialize();

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

    // Shutdown app
    Shutdown();

    // wParam of WM_QUIT is the app exit code
    return curMsg.wParam;
}

//----------------------------------------------------------------------------------------

//! Starts up the application.
//! \note Creates the dummy startup window used to initialize the OGL extensions.
void Application::StartupAppInternal()
{
    Core::Window* newWnd = nullptr;
    Core::WindowConfig baseConfig;
    Core::Window::WindowConfigPrivate privConfig(baseConfig);

    // Create window and immediately destroy it
    //! \todo Assert/log on failure
    privConfig.mHINSTANCE = mHINSTANCE;
    privConfig.mIsStartupWindow = true;
    newWnd = new Core::Window(privConfig);

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
