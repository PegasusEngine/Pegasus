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

#include "Pegasus\Window\Shared\IWindowProxy.h"

// Forward declarations
namespace Pegasus {
    namespace Window {
        class Window;
    }
}

namespace Pegasus {
namespace Window {

//! \class Proxy window class.  This class manages the actual underlying window.
class WindowProxy : public IWindowProxy
{
public:
    //! Constructor
    WindowProxy(Window* wnd);
    //! Destructor
    virtual ~WindowProxy();

    // Stateflow API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Refresh();

    // Resize API
    virtual void Resize(unsigned int width, unsigned int height);

    // Unwrap API
    virtual Window* Unwrap() const;

private:
    //! The proxied window object
    Window* mObject;
};


}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
