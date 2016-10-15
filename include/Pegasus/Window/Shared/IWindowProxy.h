/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IWindowProxy.h
//! \author David Worsham
//! \date   06th October 2013
//! \brief  Proxy interface, used by the editor and launcher to interact with an app window.

#ifndef PEGASUS_SHARED_IWINDOWPROXY_H
#define PEGASUS_SHARED_IWINDOWPROXY_H

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Window/Shared/WindowConfig.h"

// Forward declarations
namespace Pegasus {
    namespace Wnd {
        class Window;
    }
    namespace PropertyGrid {
        class IPropertyGridObjectProxy;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Wnd {

//! Proxy application interface
class IWindowProxy
{
public:
    //! Destructor
    virtual ~IWindowProxy() {};

    //! Bind the render context and render the window
    virtual void Draw() = 0;

    //! Bool value, setting if this window is enabled or disabled for drawing.
    virtual void EnableDraw(bool enabled) = 0;

    //! Resizes this window.
    //! \param width New width in pixels of the window
    //! \param height New height in pixels of the window
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    //! Gets the underlying window object from this proxy
    //! Internal use only.
    //! \return Window object.
    virtual Window* Unwrap() const = 0;

    //! Returns the state of a component. Use this to send / receive realtime messages to a window component.
    //! \return the property grid of this component. Null if there isn't one.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetComponentState(Pegasus::App::ComponentType type) = 0;

    //! Hacky, but allows control of passing by keyboard events.
    //! \param k key passed.
    //! \param isDown true if the key was pressed, otherwise if released
    virtual void HandleKeyEvent(Keys k, bool isDown) = 0;

};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_IWINDOWPROXY_H
