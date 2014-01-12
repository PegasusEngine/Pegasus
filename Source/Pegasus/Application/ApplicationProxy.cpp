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
#include "Pegasus/Application/ApplicationProxy.h"
#include "Pegasus/Application/Application.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Timeline/TimelineProxy.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Window/WindowProxy.h"
#include "Pegasus/Window/Window.h"

extern Pegasus::App::Application* CreateApplication(const Pegasus::App::ApplicationConfig& config);
extern void DestroyApplication(Pegasus::App::Application* app);

//----------------------------------------------------------------------------------------


namespace Pegasus {
namespace App {

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

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    
const char* ApplicationProxy::GetSecondaryWindowType() const
{
    return mObject->GetWindowRegistry()->GetSecondaryWindowType();
}

#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

//----------------------------------------------------------------------------------------

Wnd::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();
    Wnd::Window* wnd = mObject->AttachWindow(config);
    Wnd::WindowProxy* proxy = PG_NEW(windowAlloc, -1, "WindowProxy", Pegasus::Alloc::PG_MEM_PERM) Wnd::WindowProxy(wnd); // Wrap in proxy

    return proxy;
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(Wnd::IWindowProxy* wnd)
{
    Alloc::IAllocator* windowAlloc = Memory::GetWindowAllocator();

    mObject->DetachWindow(wnd->Unwrap()); // Unwrap and destroy proxied window
    PG_DELETE(windowAlloc, wnd);
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

void ApplicationProxy::Load()
{
    mObject->Load();
}

//----------------------------------------------------------------------------------------

Timeline::ITimelineProxy* ApplicationProxy::GetTimeline() const
{
    return mObject->GetTimeline()->GetProxy();
}


}   // namespace App
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_ENABLE_PROXIES
