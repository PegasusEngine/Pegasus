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
#include "Pegasus/Timeline/Proxy/TimelineManagerProxy.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Window/WindowProxy.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Shader/Proxy/ShaderManagerProxy.h"
#include "Pegasus/Texture/Proxy/TextureManagerProxy.h"

extern Pegasus::App::Application* CreateApplication(const Pegasus::App::ApplicationConfig& config);
extern void DestroyApplication(Pegasus::App::Application* app);

//----------------------------------------------------------------------------------------

extern void Internal_GetEngineDesc(Pegasus::PegasusDesc& engineDesc);

namespace Pegasus {
namespace App {

ApplicationProxy::ApplicationProxy(const ApplicationConfig& config)
{
    mApplication = CreateApplication(config);
}

//----------------------------------------------------------------------------------------

ApplicationProxy::~ApplicationProxy()
{
    DestroyApplication(mApplication);
}

//----------------------------------------------------------------------------------------

const char* ApplicationProxy::GetMainWindowType() const
{
    return mApplication->GetWindowRegistry()->GetMainWindowType();
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    
const char* ApplicationProxy::GetSecondaryWindowType() const
{
    return mApplication->GetWindowRegistry()->GetSecondaryWindowType();
}

//----------------------------------------------------------------------------------------

const char* ApplicationProxy::GetTextureEditorPreviewWindowType() const
{
    return mApplication->GetWindowRegistry()->GetTextureEditorPreviewWindowType();
}

#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

//----------------------------------------------------------------------------------------

Wnd::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Wnd::Window* wnd = mApplication->AttachWindow(config);
    return wnd->GetProxy();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(Wnd::IWindowProxy* wnd)
{
    mApplication->DetachWindow(wnd->Unwrap()); // Unwrap and destroy proxied window
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Initialize()
{
    mApplication->Initialize();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Shutdown()
{
    mApplication->Shutdown();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Load()
{
    mApplication->Load();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Update()
{
    mApplication->Update();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::Unload()
{
    mApplication->Unload();
}

//----------------------------------------------------------------------------------------

Timeline::ITimelineManagerProxy* ApplicationProxy::GetTimelineManagerProxy() const
{
    return mApplication->GetTimelineManager()->GetProxy();
}


//----------------------------------------------------------------------------------------

void ApplicationProxy::GetEngineDesc(Pegasus::PegasusDesc& engineDesc) const
{
    Internal_GetEngineDesc(engineDesc);
}


}   // namespace App
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif  // PEGASUS_ENABLE_PROXIES
