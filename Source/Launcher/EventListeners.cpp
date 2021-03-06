#include "Launcher/EventListeners.h"

using namespace Pegasus::Core;

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

    void LauncherShaderListener::OnEvent(Core::IEventUserData * u, CompilerEvents::SourceLoadedEvent& e)
    {
        Log('SHDR', "Shader text has been set.");
    }

    void LauncherShaderListener::OnEvent(Core::IEventUserData * u, CompilerEvents::CompilationEvent& e)
    {
        Assert(e.IsSuccess(), e.GetLogString());
    }

    void LauncherShaderListener::OnEvent(Core::IEventUserData * u, CompilerEvents::CompilationNotification& e)
    {
        //ignore notifications
    }

    void LauncherShaderListener::OnEvent(Core::IEventUserData * u, CompilerEvents::LinkingEvent& e)
    {
        Assert(e.GetEventType() == CompilerEvents::LinkingEvent::LINKING_SUCCESS, e.GetLog());
    }

    void LauncherTextureNodeListener::OnEvent(Core::IEventUserData * u, Texture::TextureNodeNotificationEvent& e)
    {
        Assert(false, e.GetMessage());
    }

    void LauncherTextureNodeListener::OnEvent(Core::IEventUserData * u, Texture::TextureNodeGenerationEvent& e)
    {
        //ignore generation notifications
    }

    void LauncherTextureNodeListener::OnEvent(Core::IEventUserData * u, Texture::TextureNodeOperationEvent& e)
    {
        //ignore operation notifications
    }

    void LauncherMeshListener::OnEvent(Core::IEventUserData * u, Mesh::MeshNotificationEvent& e)
    {
        Assert(false, e.GetMessage());
    }

    void LauncherMeshListener::OnEvent(Core::IEventUserData * u, Mesh::MeshGenerationEvent&   e)
    {
        //ignore generation notifications
    }

    void LauncherMeshListener::OnEvent(Core::IEventUserData * u, Mesh::MeshOperationEvent&    e)
    {
        //ignore operation notifications
    }

}
}
