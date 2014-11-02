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
#include "Pegasus/Texture/Shared/TextureEvent.h"
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

// The following are event listeners for each event occuring within an app
class LauncherShaderListener : public Core::CompilerEvents::ICompilerEventListener, LogAssertDispatcher
{
public:
    LauncherShaderListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherShaderListener(){}

    virtual void OnEvent(Graph::IGraphUserData * u, Core::CompilerEvents::SourceLoadedEvent& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Core::CompilerEvents::CompilationEvent& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Core::CompilerEvents::CompilationNotification& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Core::CompilerEvents::LinkingEvent& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Core::CompilerEvents::FileOperationEvent& e);
};


class LauncherTextureListener : public Texture::ITextureEventListener, LogAssertDispatcher
{
public:
    LauncherTextureListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherTextureListener(){}

    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureNotificationEvent& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureGenerationEvent&   e);
    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureOperationEvent&    e);
};


class LauncherMeshListener : public Mesh::IMeshEventListener, LogAssertDispatcher
{
public:
    LauncherMeshListener(Core::LogHandlerFunc logHandler, Core::AssertionHandlerFunc assertHandler)
    : LogAssertDispatcher(logHandler, assertHandler) {}
    ~LauncherMeshListener() {}

    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshNotificationEvent& e);
    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshGenerationEvent&   e);
    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshOperationEvent&    e);
};


} //namespace Launcher

} //namespace Pegasus

#endif
