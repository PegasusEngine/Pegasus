/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IApplicationProxy.h
//! \author David Worsham
//! \date   25th August 2013
//! \brief  Proxy interface, used by the editor and launcher to interact with an app.

#ifndef PEGASUS_SHARED_IAPPPROXY_H
#define PEGASUS_SHARED_IAPPPROXY_H

#include "Pegasus/Version.h"

#if PEGASUS_ENABLE_PROXIES
// Forward declarations
namespace Pegasus {
    namespace App {
        struct ApplicationConfig;
        struct AppWindowConfig;
#if PEGASUS_ENABLE_BS_REFLECTION_INFO
        class IAppBsReflectionInfo;
#endif
    }

    namespace PropertyGrid {
        class IPropertyGridManagerProxy;
    }

    namespace Timeline {
        class ITimelineManagerProxy;
    }
 
    namespace Wnd {
        struct AppWindowConfig;
        class IWindowProxy;
    }

    namespace Shader {
        class IShaderManagerProxy;
    }

    namespace Texture {
        class ITextureManagerProxy;
#if PEGASUS_USE_EVENTS
        class ITextureNodeEventListener;
#endif
    }

    namespace Mesh {
        class IMeshManagerProxy;
#if PEGASUS_USE_EVENTS
        class IMeshEventListener;
#endif
    }

    namespace AssetLib {
        class IAssetLibProxy;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace App {


//! Proxy application interface.
class IApplicationProxy
{
public:
    //! Destructor
    virtual ~IApplicationProxy() {};

    //! Creates a new window attached to this application instance
    //! \param config Configuration structure for the newly created window.
    //! \return Pointer to the opened window.
    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config) = 0;

    //! Destroys a window attached to this application instance
    //! \param wnd Window to destroy.
    virtual void DetachWindow(Wnd::IWindowProxy* wnd) = 0;

    //! Get the property grid manager proxy
    //! \return Property grid manager proxy
    virtual PropertyGrid::IPropertyGridManagerProxy * GetPropertyGridManagerProxy() = 0;

    //! Gets the timeline proxy
    //! \return Timeline proxy
    virtual Timeline::ITimelineManagerProxy * GetTimelineManagerProxy() = 0;

    //! Gets the shader manager, for shader and GPU program controls
    //! \return Shader manager
    virtual Shader::IShaderManagerProxy * GetShaderManagerProxy() = 0;

    //! Gets the texture manager, for texture and GPU program controls
    //! \return Texture manager
    virtual Texture::ITextureManagerProxy * GetTextureManagerProxy() = 0;

    //! Gets the mesh manager, for mesh and GPU program controls
    //! \return Mesh manager
    virtual Mesh::IMeshManagerProxy * GetMeshManagerProxy() = 0;

    //! Gets the asset lib proxy, to control assets
    //! \return asset library proxy
    virtual AssetLib::IAssetLibProxy* GetAssetLibProxy() = 0;

#if PEGASUS_ENABLE_BS_REFLECTION_INFO
    //! Returns the reflection information from blockscript.
    virtual App::IAppBsReflectionInfo* GetBsReflectionInfo() const = 0;
#endif

    //! Load the assets required to render the timeline blocks
    virtual void Load() = 0;
    //! \todo Set update mode

    //! Update function: application's sim & sound tick.
    virtual void Update() = 0;

    //! Unload the assets from render timeline block 
    virtual void Unload() = 0;
    //! \todo Set update mode

    //! Get the timeline proxy object
    //! \return Timeline proxy object
    virtual Timeline::ITimelineManagerProxy* GetTimelineManagerProxy() const = 0;

    //! Get the about struct for the application 
    //! \param appDesc the output description structure
    virtual void GetEngineDesc(Pegasus::PegasusDesc& engineDesc) const = 0;

    //! returns the root folder for asset loading
    virtual const char* GetAssetsRoot() const = 0;

};

//----------------------------------------------------------------------------------------

//! Typedef for proxy factory function
typedef Pegasus::App::IApplicationProxy* (*CreatePegasusAppFuncPtr)(const Pegasus::App::ApplicationConfig& config);
//! Typedef for proxy factory function
typedef void (*DestroyPegasusAppFuncPtr)(Pegasus::App::IApplicationProxy* app);

}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_IAPPPROXY_H
