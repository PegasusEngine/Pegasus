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

// Forward declarations
namespace Pegasus {
    namespace Application {
        class Application;
    }
    namespace Render {
        class Context;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Window {

//! Class that represents an application window
class Window
{
public:
    //! Constructor
    //! \param config Config structure for this window.
    Window(const WindowConfig& config);

    //! Destructor
    virtual ~Window();


    //! Gets the handle for this window
    //! \return Window handle.
    inline WindowHandle GetHandle() const { return mHWND; };

    //! Gets the window context for this window
    //! \return Window context.
    inline IWindowContext* GetWindowContext() { return mWindowContext; }

    //! Gets the render context used by this window
    //! \return Render context.
    inline Render::Context* GetRenderContext() const { return mRenderContext; };

    //! Gets the dimensions of this window
    //! \param width Width outParam.
    //! \param Height outParam.
    inline void GetDimensions(unsigned int& width, unsigned int& height) { width = mWidth; height = mHeight; }


    //! Initialize this window
    virtual void Initialize() = 0;

    //! Shutdown this window
    virtual void Shutdown() = 0;

    //! Trigger this window to draw one frame
    virtual void Refresh() = 0;


    //! Resize this window
    //! \param width New width in pixels.
    //! \param New height in pixels.
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
    PG_DISABLE_COPY(Window);


    //! Internal helper to create a window
    //! \param config Config struct for this window.
    void Internal_CreateWindow(const WindowConfig& config);


    WindowHandle mHWND; //!< Window handle
    IWindowContext* mWindowContext; //!< Context for this window to operate in
    bool mUseBasicContext; //!< Flag to use a basic rendering context
    Render::Context* mRenderContext; //!< Rendering context
    unsigned int mWidth; //!< Current width
    unsigned int mHeight; //!< Current height
};

//----------------------------------------------------------------------------------------

//! Factory method for windows
typedef Window* (*WindowFactoryFunc)(const WindowConfig& config);


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_WINDOW_WINDOW_H
