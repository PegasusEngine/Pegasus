/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationProxy.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app.

#ifndef PEGASUS_SHARED_APPLICATIONPROXY_H
#define PEGASUS_SHARED_APPLICATIONPROXY_H

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus\Application\Shared\IApplicationProxy.h"

// Forward declarations
namespace Pegasus {
    namespace Application {
        class Application;
    }
}

namespace Pegasus {
namespace Application {

//! \class Proxy application class.  This class manages the actual underlying application.
class ApplicationProxy : public IApplicationProxy
{
public:
    // Ctor / dtor
    ApplicationProxy(const ApplicationConfig& config);
    virtual ~ApplicationProxy();

    // Window API
    virtual const char* GetMainWindowType() const;
    virtual Window::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(const Window::IWindowProxy* wnd);

    // Stateflow API
    virtual void Initialize();
    virtual void Shutdown();

    //! \todo Set update mode
    virtual void SetAppTime(float time);

private:
    //! The proxied application object
    Application* mObject;
};


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_APPLICATION_H
