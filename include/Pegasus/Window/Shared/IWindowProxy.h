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

namespace Pegasus {
    namespace Window {
        class Window;
    }
}

namespace Pegasus {
namespace Window {

//! \class Proxy application interface.
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

    //! Requests that this window render a new frame
    virtual void Refresh() = 0;

    //! Resizes this window.
    //! \param width New width in pixels of the window
    //! \param height New height in pixels of the window
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    // Unwrap API
    virtual Window* Unwrap() const = 0;
};

}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_SHARED_IWINDOWPROXY_H
