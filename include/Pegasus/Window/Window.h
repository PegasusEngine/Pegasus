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

#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Window/Shared/WindowConfig.h"
#include "Pegasus/Window/IWindowComponent.h"

// Forward declarations
namespace Pegasus {
    namespace Core {
        class IApplicationContext;
    }
    namespace App {
        class Application;
    }
    namespace Render {
        class IDevice;
        class Context;
    }
    namespace Wnd {
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
    Os::WindowHandle GetHandle() const;

    //! Gets the device assigned to this window
    //! \return IDevice interface
    inline Render::IDevice* GetRenderDevice() const { return mDevice; }

    //! Gets the render context used by this window
    //! \return Render context.
    inline Render::Context* GetRenderContext() const { return mRenderContext; };

    //! Gets the dimensions of this window
    //! \param width Width outParam.
    //! \param Height outParam.
    inline void GetDimensions(unsigned int& width, unsigned int& height) const { width = mWidth; height = mHeight; }

    //! Get the width of this window
    //! \return Width of the window in pixels
    inline unsigned int GetWidth() const { return mWidth; }

    //! Get the height of this window
    //! \return Height of the window in pixels
    inline unsigned int GetHeight() const { return mHeight; }

    //! Get the aspect ratio of this window
    //! \return Aspect ratio window (== width / height), 1.0f if height is undefined
    inline float GetRatio() const { return mRatio; }

    //! Initialize this window
    virtual void Initialize() = 0;

    //! Shutdown this window
    virtual void Shutdown() = 0;

    //! Render the content of the window
    //! \warning Do not use directly, call \a Draw() instead to guarantee the render context is bound
    virtual void Render() = 0;

    //! Bind the render context and render the window
    void Draw();

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

    //! Attaches a window component to this window. Internally creates the state.
    //! \param component - the component to add to this window.
    void AttachComponent(IWindowComponent* component);

    //! Removes all windows components. Destroys internal states created.
    //! \param component - the component to add to this window.
    void RemoveComponents();

protected:

    //! Gets the window context for this window
    //! \return Window context.
    inline Core::IApplicationContext* GetWindowContext() { return mWindowContext; }


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
    Core::IApplicationContext* mWindowContext; //!< Context for this window to operate in
    Render::IDevice * mDevice;
    Render::Context* mRenderContext; //!< Rendering context
    bool mContextCreated; //!< Window created flag
    unsigned int mWidth; //!< Current width
    unsigned int mHeight; //!< Current height
    float mRatio; //!< Aspect ratio (== width / height), 1.0f if height is undefined
    bool mIsChild; //!< Current state, wether is child window or not
    
    struct StateComponentPair {
        IWindowComponent::IState* mState;
        IWindowComponent* mComponent;
    };
    
    Utils::Vector<StateComponentPair> mComponents;

    friend class WindowMessageHandler;
};

//----------------------------------------------------------------------------------------

//! Factory method for windows
typedef Window* (*WindowFactoryFunc)(const WindowConfig& config, Alloc::IAllocator* alloc);


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOW_H
