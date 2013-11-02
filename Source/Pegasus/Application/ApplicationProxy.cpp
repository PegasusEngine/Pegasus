/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationProxy.cpp
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app.

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus\Application\ApplicationProxy.h"
#include "Pegasus\Application\Application.h"
#include "Pegasus\Window\WindowProxy.h"
#include "Pegasus\Window\Window.h"

extern Pegasus::Application::Application* CreateApplication(const Pegasus::Application::ApplicationConfig& config);
extern void DestroyApplication(Pegasus::Application::Application* app);

//----------------------------------------------------------------------------------------


namespace Pegasus {
namespace Application {

ApplicationProxy::ApplicationProxy(const ApplicationConfig& config)
{
    mObject = CreateApplication(config);
}

//----------------------------------------------------------------------------------------

ApplicationProxy::~ApplicationProxy()
{
    DestroyApplication(mObject);
}

//----------------------------------------------------------------------------------------

const char* ApplicationProxy::GetMainWindowType() const
{
    return mObject->GetWindowRegistry()->GetMainWindowType();
}

//----------------------------------------------------------------------------------------

Window::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Window::Window* wnd = mObject->AttachWindow(config);
    Window::WindowProxy* proxy = PG_NEW("WindowProxy", Pegasus::Memory::PG_MEM_PERM) Window::WindowProxy(wnd); // Wrap in proxy

    return proxy;
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(const Window::IWindowProxy* wnd)
{
    mObject->DetachWindow(wnd->Unwrap()); // Unwrap and destroy proxied window
    PG_DELETE wnd;
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Initialize()
{
    mObject->Initialize();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Shutdown()
{
    mObject->Shutdown();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::SetAppTime(float time)
{
    mObject->SetAppTime(time);
}


}   // namespace Application
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
