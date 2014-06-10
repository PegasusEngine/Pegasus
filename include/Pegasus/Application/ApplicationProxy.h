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
#include "Pegasus/Application/Application.h"
#include "Pegasus/Timeline/TimelineProxy.h"

#if PEGASUS_USE_GRAPH_EVENTS
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Texture/Shared/TextureEvent.h"
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

    // Window API
    virtual const char* GetMainWindowType() const;
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    virtual const char* GetSecondaryWindowType() const;
    virtual const char* GetTextureEditorPreviewWindowType() const;
#endif
    virtual Wnd::IWindowProxy* AttachWindow(const AppWindowConfig& config);
    virtual void DetachWindow(Wnd::IWindowProxy* wnd);
    virtual Timeline::ITimelineProxy * GetTimelineProxy() { return mObject->GetTimeline()->GetProxy(); }
    virtual Shader::IShaderManagerProxy * GetShaderManagerProxy() { return mShaderManagerProxy; }
    virtual Texture::ITextureManagerProxy * GetTextureManagerProxy() { return mTextureManagerProxy; }

    // Stateflow API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Load();

#if PEGASUS_USE_GRAPH_EVENTS
    //! \todo Temporary. Remove as soon as the proper interfaces are defined
    virtual void RegisterMeshEventListener(Mesh::IMeshEventListener * eventListener);
#endif  // PEGASUS_USE_GRAPH_EVENTS

    // Proxy accessors
    virtual Timeline::ITimelineProxy* GetTimeline() const;

private:
    //! The proxied application object
    Application* mObject;

    //! \todo Make those proxies owned by the managers themselves (look at Timeline)
    Shader::IShaderManagerProxy * mShaderManagerProxy; 
    Texture::ITextureManagerProxy * mTextureManagerProxy; 
};


}   // namespace App
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_SHARED_APPPROXY_H
