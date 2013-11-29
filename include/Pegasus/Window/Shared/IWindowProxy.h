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
// Forward declarations
namespace Pegasus {
    namespace Wnd {
        class Window;
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


    //! Inits this window
    //! Used for window-specific initialization
    virtual void Initialize() = 0;

    //! Shuts down and destroys this window
    //! Used for window-specific shutdown
    virtual void Shutdown() = 0;

    //! Bind the render context and render the window
    virtual void Refresh() = 0;


    //! Resizes this window.
    //! \param width New width in pixels of the window
    //! \param height New height in pixels of the window
    virtual void Resize(unsigned int width, unsigned int height) = 0;


    //! Gets the underlying window object from this proxy
    //! Internal use only.
    //! \return Window object.
    virtual Window* Unwrap() const = 0;
};


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_IWINDOWPROXY_H
