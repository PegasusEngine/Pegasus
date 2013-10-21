/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.h
//! \author David Worsham
//! \date   4th July 2013
//! \brief  Class for a single window in a Pegasus application.

#ifndef PEGASUS_WINDOW_WINDOW_H
#define PEGASUS_WINDOW_WINDOW_H

#include "Pegasus/Window/WindowDefs.h"
#include "Pegasus/Window/Shared/WindowConfig.h"


namespace Pegasus {
    namespace Application {
        class Application;
    }
    namespace Render {
        class Context;
    }
}


namespace Pegasus {
namespace Window {

//! \class Class that represents an application window.
//! \todo we should create a WindowManager for multi windows
class Window
{
public:
    // Ctor / Dtor
    Window(const WindowConfig& config);
    virtual ~Window();

    // Getters
    inline WindowHandle GetHandle() const { return mHWND; };
    inline Application::Application* GetApplication() const { return mApplication; };
    inline Render::Context* GetRenderContext() const { return mRenderContext; };
    inline void GetDimensions(unsigned int& width, unsigned int& height) { width = mWidth; height = mHeight; }

    // App-specific API
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Refresh() = 0;

    // Resize API
    void Resize(unsigned int width, unsigned int height);

    // Message handling
#if PEGASUS_PLATFORM_WINDOWS
    struct HandleMessageReturn
    {
        HandleMessageReturn() : retcode(0), handled(false) {}
        ~HandleMessageReturn() {}


        unsigned int retcode;
        bool handled;
    };

    // Temp, until we make window manager
    static bool RegisterWindowClass(ModuleHandle handle);
    static bool UnregisterWindowClass(ModuleHandle handle);
    void SetHWND(WindowHandle handle) { mHWND = handle; }; //! Temp, until moved to platform specific
    HandleMessageReturn HandleMessage(unsigned int message, unsigned int* wParam, unsigned long* lParam);
#endif

private:
    // No copies allowed
    explicit Window(const Window& other);
    Window& operator=(const Window& other);

    // Helpers
    void Internal_CreateWindow(const WindowConfig& config);


    //! Application the window belongs to
    Application::Application * mApplication;

    //! Window handle
    WindowHandle mHWND;

    bool mUseBasicContext; //<! Flag to use a basic rendering context

    //! Rendering context
    Render::Context* mRenderContext;

    //! Current width
    unsigned int mWidth;

    //! Current height
    unsigned int mHeight;
};


//! Factory method for windows
typedef Window* (*WindowFactoryFunc)(const WindowConfig& config);

}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_WINDOW_H
