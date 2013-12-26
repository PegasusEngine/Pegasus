/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationProxy.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app.

#ifndef PEGASUS_SHARED_APPPROXY_H
#define PEGASUS_SHARED_APPPROXY_H

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Application/Shared/IApplicationProxy.h"

// Forward declarations
namespace Pegasus {
    namespace App {
        class Application;
    }
}

namespace Pegasus {
namespace App {

//! Proxy application class for use with a DLL
class ApplicationProxy : public IApplicationProxy
{
public:
    // Ctor / dtor
    ApplicationProxy(const ApplicationConfig& config);
    virtual ~ApplicationProxy();

    // Window API
    virtual const char* GetMainWindowType() const;
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    virtual const char* GetSecondaryWindowType() const;
#endif
    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(Wnd::IWindowProxy* wnd);

    // Stateflow API
    virtual void Initialize();
    virtual void Shutdown();

    // Proxy accessors
    virtual Timeline::ITimelineProxy* GetTimeline() const;

private:
    //! The proxied application object
    Application* mObject;
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_APPPROXY_H
