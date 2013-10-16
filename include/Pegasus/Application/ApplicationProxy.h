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
    //! Constructor
    ApplicationProxy();

    //! Destructor
    virtual ~ApplicationProxy();

    // Window API
    virtual Window::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(const Window::IWindowProxy* wnd);
    virtual void ResizeWindow(Window::IWindowProxy* wnd, int width, int height);

    // Stateflow API
    virtual void Initialize(const ApplicationConfig& config);
    virtual void Shutdown();
    virtual int Run();

    //! \todo Set update mode
    virtual void SetAppTime(float time);

    // Debug API
#if PEGASUS_ENABLE_LOG
    virtual void RegisterLogHandler(Core::LogManager::Handler handler);
#endif
#if PEGASUS_ENABLE_ASSERT
    virtual void RegisterAssertionHandler(Core::AssertionManager::Handler handler);
    virtual void InvalidateWindows();
#endif

    // Render API
    virtual void Resize(const Window::IWindowProxy* wnd, int width, int height);
    virtual void Render();

private:
    //! The proxied application object
    Application* mObject;
};


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_APPLICATION_H
