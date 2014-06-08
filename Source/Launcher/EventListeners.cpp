#include "Launcher/EventListeners.h"

namespace Pegasus
{
namespace Launcher
{

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::ShaderLoadedEvent)
    {
        //TODO handle this event
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::CompilationEvent)
    {
        //TODO handle this event
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::CompilationNotification)
    {
        //TODO handle this event
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::LinkingEvent)
    {
        //TODO handle this event
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::FileOperationEvent)
    {
        //TODO handle this event
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureNotificationEvent& e)
    {
        //TODO handle this event
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureGenerationEvent&   e)
    {
        //TODO handle this event
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureOperationEvent&    e)
    {
        //TODO handle this event
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshNotificationEvent& e)
    {
        //TODO handle this event
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshGenerationEvent&   e)
    {
        //TODO handle this event
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshOperationEvent&    e)
    {
        //TODO handle this event
    }

}
}
