/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphIOMessageController.cpp
//! \author	Karolyn Boulanger
//! \date	December 25th 2015
//! \brief	Graph IO controller, communicates events from the graph editor
//!         in a thread-safe way

#include "Pegasus/PegasusAssetTypes.h"
#include "MessageControllers/GraphIOMessageController.h"
#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Viewport/ViewportWidget.h"
#include <qmetatype>
#include <QVariant>


GraphIOMessageController::GraphIOMessageController(Pegasus::App::IApplicationProxy* appProxy)
:   mAppProxy(appProxy)
{
}

//----------------------------------------------------------------------------------------

GraphIOMessageController::~GraphIOMessageController()
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnRenderThreadProcessMessage(const GraphIOMCMessage& m)
{
    switch(m.GetMessageType())
    {
    case GraphIOMCMessage::VIEW_GRAPH_ON_VIEWPORT:
        OnRenderThreadViewGraphOnViewport(m.GetTargetViewport(), m.GetGraphHandle());
        break;
    }
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::FlushAllPendingUpdates()
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeNotificationEvent& e)
{   
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeGenerationEvent& e)
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnInitUserData(Pegasus::Texture::ITextureNodeProxy* proxy, const char* name)
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnDestroyUserData(Pegasus::Texture::ITextureNodeProxy* proxy, const char* name)
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeOperationEvent& e)
{
}

//----------------------------------------------------------------------------------------

QVariant GraphIOMessageController::TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    return QVariant();
}
//----------------------------------------------------------------------------------------

const Pegasus::PegasusAssetTypeDesc** GraphIOMessageController::GetTypeList() const
{
    static const Pegasus::PegasusAssetTypeDesc* gAssetList[] = {
        //&Pegasus::ASSET_TYPE_MESH, TODO: unlock mesh once it shares a common proxy.
        &Pegasus::ASSET_TYPE_TEXTURE,
        nullptr
    };
    return gAssetList;
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnRenderThreadViewGraphOnViewport(ViewportWidget* viewport, AssetInstanceHandle graphHandle)
{
    Pegasus::Wnd::IWindowProxy* windowProxy = viewport->GetWindowProxy();
    if (windowProxy != nullptr)
    {
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* graphProxy = FindInstance(graphHandle);
        if (graphProxy != nullptr)
        {
            mAppProxy->SetDebugWindowResource(windowProxy, graphProxy);
            windowProxy->Draw();
        }
    }
}

//----------------------------------------------------------------------------------------

GraphNodeObserver::GraphNodeObserver()
{
    connect(this, SIGNAL(OnInitializedSignal()),
            this, SLOT(OnInitializedSlot()),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnUpdatedSignal()),
            this, SLOT(OnUpdatedSlot()),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnShutdownSignal()),
            this, SLOT(OnShutdownSlot()),
            Qt::QueuedConnection);
}
