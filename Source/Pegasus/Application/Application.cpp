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

#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/AppWindowManager.h"
#include "Pegasus/Render/GL/GLExtensions.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Window/StartupWindow.h"
#include <windows.h>
#undef CreateWindow

namespace Pegasus {
namespace Application {

// Constants
const char* STARTUP_WINDOW_TYPE = "INTERNAL__Startup";

//----------------------------------------------------------------------------------------

Application::Application(const ApplicationConfig& config)
    : mInitialized(false), mModuleHandle(config.mModuleHandle), mAppTime(0.0f)
{
    AppWindowManagerConfig managerConfig;
    WindowRegistration reg;

    // Set up debugging facilities
#if PEGASUS_ENABLE_LOG
    Core::LogManager::GetInstance().RegisterHandler(config.mLoghandler);
#endif
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::GetInstance().RegisterHandler(config.mAssertHandler);
#endif

    // Set up manager
    managerConfig.mMaxWindowTypes = config.mMaxWindowTypes;
    managerConfig.mMaxNumWindows = config.mMaxNumWindows;
    mWindowManager = PG_CORE_NEW("AppWindowManager", Pegasus::Memory::PG_MEM_PERM) AppWindowManager(managerConfig);

    // Register startup window
    reg.mTypeTag = Pegasus::Application::WINDOW_TYPE_INVALID;
    reg.mDescription = "INTERNAL Startup Window";
    reg.mCreateFunc = Window::StartupWindow::Create;
    mWindowManager->RegisterWindowClass(STARTUP_WINDOW_TYPE, reg);
}

//----------------------------------------------------------------------------------------

Application::~Application()
{
    // Sanity check
    PG_ASSERTSTR(!mInitialized, "Application still initialized in destructor!");

    // Unregister startup window
    mWindowManager->UnregisterWindowClass(STARTUP_WINDOW_TYPE);

    // Free windows
    PG_DELETE mWindowManager;

    // Tear down debugging facilities
#if PEGASUS_ENABLE_LOG
    Core::LogManager::GetInstance().UnregisterHandler();
#endif
#if PEGASUS_ENABLE_ASSERT
    Core::AssertionManager::GetInstance().UnregisterHandler();
#endif
}

//----------------------------------------------------------------------------------------

void Application::Initialize()
{
    Window::Window* startupWindow = nullptr;

    // Sanity check
    PG_ASSERTSTR(!mInitialized, "Application already initialized!");

    // start up the app, which creates and destroys the dummy window
    StartupAppInternal();

    // Initted
    mInitialized = true;
}

//----------------------------------------------------------------------------------------

//! Shutdown this application.
void Application::Shutdown()
{
    // Sanity check
    PG_ASSERTSTR(mInitialized, "Application not initialized yet!");

    // Shuts down GLextensions, etc
    ShutdownAppInternal();

    // No longer initted
    mInitialized = false;
}

//----------------------------------------------------------------------------------------

IWindowRegistry* Application::GetWindowRegistry()
{
    return mWindowManager;
}

//----------------------------------------------------------------------------------------

Window::Window* Application::AttachWindow(const AppWindowConfig& appWindowConfig)
{
    Window::Window* newWnd = nullptr;
    Window::WindowConfig config;

    // Create window
    config.mModuleHandle = mModuleHandle;
    config.mApplication = this;
    config.mIsChild = appWindowConfig.mIsChild;
    config.mParentWindowHandle = appWindowConfig.mParentWindowHandle;
    config.mWidth = appWindowConfig.mWidth;
    config.mHeight = appWindowConfig.mHeight;
    config.mCreateVisible = true;
    config.mUseBasicContext = false;
    newWnd = mWindowManager->CreateWindow(appWindowConfig.mWindowType, config);
    PG_ASSERTSTR(newWnd != nullptr, "Unable to create window!");

    return newWnd;
}

//----------------------------------------------------------------------------------------

void Application::DetachWindow(const Window::Window* wnd)
{
    mWindowManager->DestroyWindow(wnd);
}

//----------------------------------------------------------------------------------------

//! Starts up the application.
//! \note Creates the dummy startup window used to initialize the OGL extensions.
void Application::StartupAppInternal()
{
    Window::Window* newWnd = nullptr;
    Window::WindowConfig config;

    // First register window classes
    Window::Window::RegisterWindowClass(mModuleHandle);

    // Create window and immediately destroy it
    //! \todo Assert/log on failure
    config.mModuleHandle = mModuleHandle;
    config.mApplication = this;
    config.mIsChild = false;
    config.mWidth = 128;
    config.mHeight = 128;
    config.mCreateVisible = false;
    config.mUseBasicContext = true;
    newWnd = mWindowManager->CreateWindow(STARTUP_WINDOW_TYPE, config);

    // Init openGL extensions now that we have a context
    Render::GLExtensions::CreateInstance();

    // Write some temporary debugging information
    //! \todo Remove this after testing
    Render::GLExtensions & extensions = Render::GLExtensions::GetInstance();
    switch (extensions.GetMaximumProfile())
    {
    case Render::GLExtensions::PROFILE_GL_3_3:
        PG_LOG('OGL_', "OpenGL 3.3 is the maximum detected profile.");
        break;

    case Render::GLExtensions::PROFILE_GL_4_3:
        PG_LOG('OGL_', "OpenGL 4.3 is the maximum detected profile.");
        break;

    default:
        PG_LOG('OGL_', "Error when initializing GLExtensions.");
        break;
    }
    if (extensions.IsGLExtensionSupported("GL_ARB_draw_indirect"))
    {
        PG_LOG('OGL_', "GL_ARB_draw_indirect detected.");
    }
    else
    {
        PG_LOG('OGL_', "GL_ARB_draw_indirect NOT detected.");
    }
    if (extensions.IsGLExtensionSupported("GL_ATI_fragment_shader"))
    {
        PG_LOG('OGL_', "GL_ATI_fragment_shader detected.");
    }
    else
    {
        PG_LOG('OGL_', "GL_ATI_fragment_shader NOT detected.");
    }
    if (extensions.IsWGLExtensionSupported("WGL_ARB_buffer_region"))
    {
        PG_LOG('OGL_', "WGL_ARB_buffer_region detected.");
    }
    else
    {
        PG_LOG('OGL_', "WGL_ARB_buffer_region NOT detected.");
    }
    if (extensions.IsWGLExtensionSupported("WGL_3DL_stereo_control"))
    {
        PG_LOG('OGL_', "WGL_3DL_stereo_control detected.");
    }
    else
    {
        PG_LOG('OGL_', "WGL_3DL_stereo_control NOT detected.");
    }

    // Destroy the window, it is no longer needed
    mWindowManager->DestroyWindow(newWnd);
}

//----------------------------------------------------------------------------------------

//! shuts down the application.
void Application::ShutdownAppInternal()
{
    // Destroy openGL extensions
    Render::GLExtensions::DestroyInstance();

    // Unregister window classes
    Window::Window::UnregisterWindowClass(mModuleHandle);
}

//----------------------------------------------------------------------------------------

}   // namespace Application
}   // namespace Pegasus
