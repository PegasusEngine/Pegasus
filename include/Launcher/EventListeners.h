/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	EventListeners.h
//! \author	Kleber Garcia
//! \date	8th June 2014
//! \brief	event listener classes to trigger asserts on application events

#ifndef PEGASUS_EVENTLISTENERS_H
#define PEGASUS_EVENTLISTENERS_H

#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Texture/Shared/TextureEventDefs.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Shared/AssertReturnCode.h"

namespace Pegasus
{

namespace Launcher
{

class LogAssertDispatcher
{
public:
    LogAssertDispatcher(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : mLogHandler(logHandler), mAssertHandler(assertHandler)
    {
    }
    ~LogAssertDispatcher()
    {
    }

    void Assert(bool condition, const char * msg);
    void Log(Pegasus::Core::LogChannel channel, const char * msg);
private:
    Core::LogHandlerFunc mLogHandler;
    Core::AssertionHandlerFunc mAssertHandler;
    
};

// The following are event listeners for each event occurring within an app
class LauncherShaderListener : public Core::CompilerEvents::ICompilerEventListener, LogAssertDispatcher
{
public:
    LauncherShaderListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherShaderListener(){}

    virtual void OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name){}
    virtual void OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name){}
    virtual void OnEvent(Core::IEventUserData * u, Core::CompilerEvents::SourceLoadedEvent& e);
    virtual void OnEvent(Core::IEventUserData * u, Core::CompilerEvents::CompilationEvent& e);
    virtual void OnEvent(Core::IEventUserData * u, Core::CompilerEvents::CompilationNotification& e);
    virtual void OnEvent(Core::IEventUserData * u, Core::CompilerEvents::LinkingEvent& e);
};


class LauncherTextureNodeListener : public Texture::ITextureNodeEventListener, LogAssertDispatcher
{
public:
    LauncherTextureNodeListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherTextureNodeListener(){}

    virtual void OnEvent(Core::IEventUserData * u, Texture::TextureNodeNotificationEvent& e);
    virtual void OnEvent(Core::IEventUserData * u, Texture::TextureNodeGenerationEvent& e);
    virtual void OnEvent(Core::IEventUserData * u, Texture::TextureNodeOperationEvent& e);
};


class LauncherMeshListener : public Mesh::IMeshEventListener, LogAssertDispatcher
{
public:
    LauncherMeshListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherMeshListener() {}

    virtual void  OnEvent(Core::IEventUserData * u, Mesh::MeshNotificationEvent& e);
    virtual void  OnEvent(Core::IEventUserData * u, Mesh::MeshGenerationEvent& e);
    virtual void  OnEvent(Core::IEventUserData * u, Mesh::MeshOperationEvent& e);
};


} //namespace Launcher

} //namespace Pegasus

#endif
