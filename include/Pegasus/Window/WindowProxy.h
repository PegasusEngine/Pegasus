/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowProxy.h
//! \author David Worsham
//! \date   06th October 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app window.

#ifndef PEGASUS_SHARED_WINDOWPROXY_H
#define PEGASUS_SHARED_WINDOWPROXY_H

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Window/Shared/IWindowProxy.h"

// Forward declarations
namespace Pegasus {
    namespace Wnd {
        class Window;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Wnd {

//! Proxy window class for use with a DLL
class WindowProxy : public IWindowProxy
{
public:
    //! Constructor
    //! \param wnd Window to wrap.
    WindowProxy(Window* wnd);

    //! Destructor
    virtual ~WindowProxy();

    // Stateflow API
    virtual void Draw();

    //! Bool value, setting if this window is enabled or disabled for drawing.
    virtual void EnableDraw(bool enabled);

    // Resize API
    virtual void Resize(unsigned int width, unsigned int height);

    // Window component state, to control component behaviour.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetComponentState(Pegasus::App::ComponentType type);

    // Unwrap API
    virtual Window* Unwrap() const;

    //! Hacky, but allows control of passing by keyboard events.
    //! \param k key passed.
    //! \param isDown true if the key was pressed, otherwise if released
    virtual void HandleKeyEvent(Keys k, bool isDown);

private:
    //! The proxied window object
    Window* mObject;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_APP_H
