#include "Launcher/EventListeners.h"

namespace Pegasus
{
namespace Launcher
{

    void LogAssertDispatcher::Assert(bool condition, const char * msg)
    {
        if (!condition)
        {
            mAssertHandler("","",0,msg);
        }
    }

    void LogAssertDispatcher::Log(Pegasus::Core::LogChannel channel, const char * msg)
    {
        mLogHandler(channel, msg);
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::ShaderLoadedEvent& e)
    {
        Log('SHDR', "Shader text has been set.");
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::CompilationEvent& e)
    {
        Assert(e.IsSuccess(), e.GetLogString());
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::CompilationNotification& e)
    {
        //ignore notifications
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::LinkingEvent& e)
    {
        Assert(e.GetEventType() == Shader::LinkingEvent::LINKING_SUCCESS, e.GetLog());
    }

    void LauncherShaderListener::OnEvent(Graph::IGraphUserData * u, Shader::FileOperationEvent& e)
    {
        Assert(false, "Pegasus Shader IO Error.");
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureNotificationEvent& e)
    {
        Assert(false, e.GetMessage());
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureGenerationEvent&   e)
    {
        //ignore generation notifications
    }

    void LauncherTextureListener::OnEvent(Graph::IGraphUserData * u, Texture::TextureOperationEvent&    e)
    {
        //ignore operation notifications
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshNotificationEvent& e)
    {
        Assert(false, e.GetMessage());
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshGenerationEvent&   e)
    {
        //ignore generation notifications
    }

    void LauncherMeshListener::OnEvent(Graph::IGraphUserData * u, Mesh::MeshOperationEvent&    e)
    {
        //ignore operation notifications
    }

}
}
