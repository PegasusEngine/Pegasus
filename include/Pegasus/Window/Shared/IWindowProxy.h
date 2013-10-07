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

    // Unwrap API
    virtual Window* Unwrap() const = 0;
};

}   // namespace Window
}   // namespace Pegasus

#endif  // PEGASUS_SHARED_IWINDOWPROXY_H
