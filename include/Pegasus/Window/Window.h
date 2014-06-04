/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.h
//! \author David Worsham
//! \date   04th July 2013
//! \brief  Class for a single window in a Pegasus application.

#ifndef PEGASUS_WND_WINDOW_H
#define PEGASUS_WND_WINDOW_H

#include "Pegasus/Window/Shared/WindowConfig.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class Application;
    }
    namespace Render {
        class Context;
    }
    namespace Wnd {
        class IWindowContext;
        class IWindowImpl;
        class WindowMessageHandler;
        class WindowProxy;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Wnd {

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
    WindowHandle GetHandle() const;

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

    //! Render the content of the window
    //! \warning Do not use directly, call \a Refresh() instead to guarantee the render context is bound
    virtual void Render() = 0;

    //! Bind the render context and render the window
    //! \param updateTimeline True to update the timeline, so animation happens
    void Refresh(bool updateTimeline);


    //! Resize this window
    //! \param New width in pixels.
    //! \param New height in pixels.
    void Resize(unsigned int width, unsigned int height);

    //! Initialize special events for a window that is a primary (controls lifetime of application)
    void HandleMainWindowEvents();


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the window
    inline WindowProxy* GetProxy() const { return mProxy; }

#endif  // PEGASUS_ENABLE_PROXIES


protected:

    //! Gets the window context for this window
    //! \return Window context.
    inline IWindowContext* GetWindowContext() { return mWindowContext; }


private:
    // No copies allowed
    PG_DISABLE_COPY(Window);

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the window
    WindowProxy * mProxy;

#endif  // PEGASUS_ENABLE_PROXIES

    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this window
    Alloc::IAllocator* mRenderAllocator; //!< Allocator to use when creating this window's render resources
    IWindowImpl* mPrivateImpl; //!< Private implementation, platform-specific
    WindowMessageHandler* mMessageHandler; //!< Message handler object
    IWindowContext* mWindowContext; //!< Context for this window to operate in
    Render::Context* mRenderContext; //!< Rendering context
    bool mContextCreated; //!< Window created flag
    unsigned int mWidth; //!< Current width
    unsigned int mHeight; //!< Current height
    bool mIsChild; //!< Current state, wether is child window or not

    friend class WindowMessageHandler;
};

//----------------------------------------------------------------------------------------

//! Factory method for windows
typedef Window* (*WindowFactoryFunc)(const WindowConfig& config, Alloc::IAllocator* alloc);


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOW_H
