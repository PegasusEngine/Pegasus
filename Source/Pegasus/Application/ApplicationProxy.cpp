/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ApplicationProxy.cpp
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy object, used by the editor and launcher to interact with an app.

#include "Pegasus\Application\ApplicationProxy.h"
#include "Pegasus\Application\Application.h"
#include "Pegasus\Window\WindowProxy.h"
#include "Pegasus\Window\Window.h"

extern Pegasus::Application::Application* CreateApplication();
extern void DestroyApplication(Pegasus::Application::Application* app);

namespace Pegasus {
namespace Application {

ApplicationProxy::ApplicationProxy()
{
    mObject = CreateApplication();
}

//----------------------------------------------------------------------------------------

ApplicationProxy::~ApplicationProxy()
{
    DestroyApplication(mObject);
}

//----------------------------------------------------------------------------------------

Window::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Window::Window* wnd = mObject->AttachWindow(config);
    Window::WindowProxy* proxy = new Window::WindowProxy(wnd); // Wrap in proxy

    return proxy;
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(const Window::IWindowProxy* wnd)
{
    mObject->DetachWindow(wnd->Unwrap()); // Unwrap and destroy proxied window
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::ResizeWindow(Window::IWindowProxy* wnd, int width, int height)
{
    mObject->ResizeWindow(wnd->Unwrap(), width, height);
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Initialize(const ApplicationConfig& config)
{
    mObject->Initialize(config);
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Shutdown()
{
    mObject->Shutdown();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Resize(const Window::IWindowProxy* wnd, int width, int height)
{
    return mObject->Resize(wnd->Unwrap(), width, height);
}

//----------------------------------------------------------------------------------------

int ApplicationProxy::Run()
{
    return mObject->Run();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::SetAppTime(float time)
{
    mObject->SetAppTime(time);
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Render()
{
    mObject->Render();
}

//----------------------------------------------------------------------------------------

}   // namespace Application
}   // namespace Pegasus
