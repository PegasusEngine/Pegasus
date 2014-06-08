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

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Texture/Shared/TextureEvent.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"

namespace Pegasus
{

namespace Launcher
{

// The following are event listeners for each event occuring within an app
class LauncherShaderListener : public Shader::IShaderEventListener
{
public:
    virtual void OnEvent(Graph::IGraphUserData * u, Shader::ShaderLoadedEvent);
    virtual void OnEvent(Graph::IGraphUserData * u, Shader::CompilationEvent);
    virtual void OnEvent(Graph::IGraphUserData * u, Shader::CompilationNotification);
    virtual void OnEvent(Graph::IGraphUserData * u, Shader::LinkingEvent);
    virtual void OnEvent(Graph::IGraphUserData * u, Shader::FileOperationEvent);
};


class LauncherTextureListener : public Texture::ITextureEventListener
{
public:
    LauncherTextureListener() {}
    ~LauncherTextureListener(){}

    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureNotificationEvent& e);
    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureGenerationEvent&   e);
    virtual void OnEvent(Graph::IGraphUserData * u, Texture::TextureOperationEvent&    e);
};


class LauncherMeshListener : public Mesh::IMeshEventListener
{
public:
    LauncherMeshListener() {}
    ~LauncherMeshListener() {}

    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshNotificationEvent& e);
    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshGenerationEvent&   e);
    virtual void  OnEvent(Graph::IGraphUserData * u, Mesh::MeshOperationEvent&    e);
};


} //namespace Launcher

} //namespace Pegasus

#endif
