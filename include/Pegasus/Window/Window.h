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
        class IWindowMessageHandler;
        class WindowProxy;
        class WindowComponentState;
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
    //! \param windowIndex, the index of this window.
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

    //! Get the inverted aspect ratio of this window
    //! \return Aspect ratio window (== height / width), 1.0f if width is undefined
    inline float GetRatioInv() const { return mRatioInv; }

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

    //! Mouse & editor event callbacks. Called from the editor.
    //! \param button - 1, 2 or 3, 1 is left, 2 is middle, 3 is right click.
    //! \param isDown true if the mouse is down, false otherwise
    //! \param x coordinate of the window, in texture coordinates [0, 1]
    //! \param y coordinate of the window, in texture coordintes [0, 1], where 1 is the top.
    void OnMouseEvent(IWindowComponent::MouseButton button, bool isDown, float x, float y);

    //! Keyboard event, when a key is pressed. Called from the editor.
    //! \param key ascii value of the key pressed.
    //! \param isDown true if key is down, false if key goes up.
    void OnKeyEvent(Pegasus::Wnd::Keys key, bool isDown);

    //! Boolean determining if this window allows drawing. Used to prevent the OS from calling draw before the systems are correclty
    //! initialized. Only required for the strange drawing behaviour of the editor.
    void EnableDraw(bool enableDraw) { mEnableDraw = enableDraw; }

#endif  // PEGASUS_ENABLE_PROXIES

    //! Attaches a window component to this window. Internally creates the state.
    //! \param component - the component to add to this window.
    void AttachComponent(IWindowComponent* component);

    //! Removes all windows components. Destroys internal states created.
    //! \param component - the component to add to this window.
    void RemoveComponents();

    struct StateComponentPair {
        WindowComponentState* mState;
        IWindowComponent* mComponent;
    };

    const Utils::Vector<StateComponentPair>& GetComponents() const { return mComponents; }


    IWindowMessageHandler* GetMessageHandler() { return mMessageHandler; }

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
    
    //! Boolean, proxy only, to stop drawing.
    bool mEnableDraw;

#endif  // PEGASUS_ENABLE_PROXIES

    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this window
    Alloc::IAllocator* mRenderAllocator; //!< Allocator to use when creating this window's render resources
    IWindowImpl* mPrivateImpl; //!< Private implementation, platform-specific
    IWindowMessageHandler* mMessageHandler; //!< Message handler object
    Core::IApplicationContext* mWindowContext; //!< Context for this window to operate in
    Render::IDevice * mDevice;
    Render::Context* mRenderContext; //!< Rendering context
    bool mContextCreated; //!< Window created flag
    unsigned int mWidth; //!< Current width
    unsigned int mHeight; //!< Current height
    float mRatio; //!< Aspect ratio (== width / height), 1.0f if height is undefined
    float mRatioInv; //!< Aspect ratio (== height / width), 1.0f if width is undefined
    bool mIsChild; //!< Current state, wether is child window or not
    
    Utils::Vector<StateComponentPair> mComponents;

    friend class WindowMessageHandler;
};

//----------------------------------------------------------------------------------------

}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOW_H
