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
#include "Pegasus/Shader/ShaderManagerProxy.h"

extern Pegasus::App::Application* CreateApplication(const Pegasus::App::ApplicationConfig& config);
extern void DestroyApplication(Pegasus::App::Application* app);

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {

ApplicationProxy::ApplicationProxy(const ApplicationConfig& config)
{
    mObject = CreateApplication(config);
    mShaderManager = PG_NEW( 
             Pegasus::Memory::GetGlobalAllocator(), 
             -1, 
            "ShaderManagerProxy", 
             Pegasus::Alloc::PG_MEM_PERM
             ) Pegasus::Shader::ShaderManagerProxy(mObject->GetShaderManager());
}

//----------------------------------------------------------------------------------------

ApplicationProxy::~ApplicationProxy()
{
    PG_DELETE(Pegasus::Memory::GetGlobalAllocator(), mShaderManager);
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

//----------------------------------------------------------------------------------------

const char* ApplicationProxy::GetTextureEditorPreviewWindowType() const
{
    return mObject->GetWindowRegistry()->GetTextureEditorPreviewWindowType();
}

#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES

//----------------------------------------------------------------------------------------

Wnd::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Wnd::Window* wnd = mObject->AttachWindow(config);
    return wnd->GetProxy();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(Wnd::IWindowProxy* wnd)
{
    mObject->DetachWindow(wnd->Unwrap()); // Unwrap and destroy proxied window
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

//! \todo Temporary. Remove as soon as the proper interface is defined
unsigned int ApplicationProxy::GetTextures(void * textureList)
{
    return mObject->GetTextures(textureList);
}

//----------------------------------------------------------------------------------------

#if PEGASUS_USE_GRAPH_EVENTS

//! todo remove these functions once we have the corresponding managers proxies?
void ApplicationProxy::RegisterTextureEventListener(Texture::ITextureEventListener * eventListener)
{
    mObject->GetTextureManager()->RegisterEventListener(eventListener);
}

void ApplicationProxy::RegisterMeshEventListener(Mesh::IMeshEventListener * eventListener)
{
    mObject->GetMeshManager()->RegisterEventListener(eventListener);
}

#endif

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
