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

#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Application/Application.h"
#include "Pegasus/Application/AppBsReflectionInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/Timeline/Proxy/TimelineManagerProxy.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Shader/ShaderManager.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Texture/TextureManager.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"



#if PEGASUS_USE_EVENTS
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Texture/Shared/TextureEventDefs.h"
#endif

namespace Pegasus {
namespace App {


//! Proxy application class for use with a DLL
class ApplicationProxy : public IApplicationProxy
{
public:
    // Ctor / dtor
    ApplicationProxy(const ApplicationConfig& config);
    virtual ~ApplicationProxy();

    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(Wnd::IWindowProxy* wnd);
    virtual PropertyGrid::IPropertyGridManagerProxy * GetPropertyGridManagerProxy() { return PropertyGrid::PropertyGridManager::GetInstance().GetProxy(); }
    virtual Timeline::ITimelineManagerProxy * GetTimelineManagerProxy() { return mApplication->GetTimelineManager()->GetProxy(); }
    virtual Shader::IShaderManagerProxy * GetShaderManagerProxy() { return mApplication->GetShaderManager()->GetProxy(); }
    virtual Texture::ITextureManagerProxy * GetTextureManagerProxy() { return mApplication->GetTextureManager()->GetProxy(); }
    virtual Mesh::IMeshManagerProxy * GetMeshManagerProxy() { return mApplication->GetMeshManager()->GetProxy(); }
    virtual AssetLib::IAssetLibProxy* GetAssetLibProxy() { return mApplication->GetAssetLib()->GetProxy(); }

#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    //! Returns the reflection information from blockscript.
    virtual App::IAppBsReflectionInfo* GetBsReflectionInfo() const { return mApplication->GetBsReflectionInfo(); }
#endif

    // Stateflow API
    virtual void Load();
    virtual void Update();
    virtual void Unload();

    // Proxy accessors
    virtual Timeline::ITimelineManagerProxy* GetTimelineManagerProxy() const;

    //! Get the about struct for the application 
    //! \param engineDesc the output description structure
    virtual void GetEngineDesc(Pegasus::PegasusDesc& engineDesc) const;

    //! returns the assets root folder
    virtual const char* GetAssetsRoot() const { return mApplication->GetIOManager()->GetRoot(); }

    //! Sets a resource for this window to debug. Must be a window with a texture / mesh view component.
    //! \param the window proxy to inject the resource into
    //! \param resource the resource to put into this mesh.
    virtual void SetDebugWindowResource(Pegasus::Wnd::IWindowProxy* window, Pegasus::AssetLib::IRuntimeAssetObjectProxy* resource);
private:
    //! The proxied application object
    Application* mApplication;
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_APPPROXY_H
