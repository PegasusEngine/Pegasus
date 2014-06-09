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
#if PEGASUS_USE_GRAPH_EVENTS
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Texture/Shared/TextureEvent.h"
#endif

// Forward declarations
namespace Pegasus {
    namespace App {
        class Application;
    }
    
    namespace Shader {
        class IShaderManagerProxy;
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
    virtual const char* GetTextureEditorPreviewWindowType() const;
#endif
    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(Wnd::IWindowProxy* wnd);
    virtual Shader::IShaderManagerProxy * GetShaderManager() { return mShaderManager; }

    // Stateflow API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Load();

    //! \todo Temporary. Remove as soon as the proper interfaces are defined
    virtual unsigned int GetTextures(void * textureList);
#if PEGASUS_USE_GRAPH_EVENTS

    //! \todo Temporary. Remove as soon as the proper interfaces are defined
    virtual void RegisterTextureEventListener(Texture::ITextureEventListener * eventListener);

    //! \todo Temporary. Remove as soon as the proper interfaces are defined
    virtual void RegisterMeshEventListener(Mesh::IMeshEventListener * eventListener);
#endif

    // Proxy accessors
    virtual Timeline::ITimelineProxy* GetTimeline() const;

private:
    //! The proxied application object
    Application* mObject;
    Shader::IShaderManagerProxy * mShaderManager; 
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_APPPROXY_H
