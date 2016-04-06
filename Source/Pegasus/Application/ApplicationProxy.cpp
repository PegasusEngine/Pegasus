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
#include "Pegasus/AssetLib/Proxy/RuntimeAssetObjectProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/Shader/Proxy/ShaderManagerProxy.h"
#include "Pegasus/Texture/Proxy/TextureManagerProxy.h"
#include "Pegasus/Application/AppWindowManager.h"
#include "Pegasus/Application/AppWindowComponentFactory.h"
#include "Pegasus/Application/Components/EditorComponents.h"

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

Wnd::IWindowProxy* ApplicationProxy::AttachWindow(const AppWindowConfig& config)
{
    Wnd::Window* wnd = mApplication->AttachWindow(config);
    return wnd->GetProxy();
}

//----------------------------------------------------------------------------------------

void ApplicationProxy::DetachWindow(Wnd::IWindowProxy* wnd)
{
    mApplication->DetachWindow(static_cast<Pegasus::Wnd::WindowProxy*>(wnd)->Unwrap()); // Unwrap and destroy proxied window
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

void ApplicationProxy::SetDebugWindowResource(Pegasus::Wnd::IWindowProxy* windowP, Pegasus::AssetLib::IRuntimeAssetObjectProxy* resourceP)
{
    Pegasus::Wnd::Window* window = static_cast<Pegasus::Wnd::WindowProxy*>(windowP)->Unwrap();
    Pegasus::AssetLib::IAssetProxy* assetProxy  = static_cast<Pegasus::AssetLib::RuntimeAssetObjectProxy*>(resourceP)->GetOwnerAsset();
    
    if (assetProxy != nullptr)
    {
        Pegasus::AssetLib::Asset* asset = static_cast<Pegasus::AssetLib::AssetProxy*>(assetProxy)->GetObject();
        const Pegasus::Utils::Vector<Pegasus::Wnd::Window::StateComponentPair>& windowComponents = window->GetComponents();
        const Pegasus::PegasusAssetTypeDesc* typeDesc = asset->GetTypeDesc();
        
        //Which type is it?
        if (typeDesc->mTypeGuid == Pegasus::ASSET_TYPE_TEXTURE.mTypeGuid)
        {
            // inject it in the texture component state of this window
            Pegasus::Wnd::IWindowComponent* textureComponent = mApplication->GetWindowManager()->GetComponentFactory()->GetComponent(COMPONENT_TEXTURE_VIEW);
            Pegasus::App::TextureViewComponentState* state = nullptr;

            for (unsigned i = 0; i < windowComponents.GetSize() && state == nullptr; ++i)
            {
                if (textureComponent == windowComponents[i].mComponent)
                {
                    state = static_cast<Pegasus::App::TextureViewComponentState*>(windowComponents[i].mState);
                }
            }

            //find the component
            if (state != nullptr)
            {
                Pegasus::Texture::TextureRef texture = (Pegasus::Texture::Texture*)(asset->GetRuntimeData());
                state->mTargetTexture = texture;
            }
        }
        else if (typeDesc->mTypeGuid == Pegasus::ASSET_TYPE_MESH.mTypeGuid)
        {
            //TODO: do work for mesh
        }
    }
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
