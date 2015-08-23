#include "MessageControllers/AssetIOMessageController.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include <QByteArray>

AssetIOMessageController::AssetIOMessageController(Pegasus::App::IApplicationProxy* app)
: mApp(app)
{
}

AssetIOMessageController::~AssetIOMessageController()
{
}

void AssetIOMessageController::OnRenderThreadProcessMessage(PegasusDockWidget* sender, const AssetIOMessageController::Message& msg)
{
    switch(msg.GetMessageType())
    {
        case AssetIOMessageController::Message::OPEN_ASSET:
            {
                OnRenderRequestOpenObject(msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::CLOSE_ASSET:
            {
                OnRenderRequestCloseObject(msg.GetObject());
            }
            break;
        case AssetIOMessageController::Message::SAVE_ASSET:
            {
                OnSaveObject(sender, msg.GetObject());
            }
            break;
        case AssetIOMessageController::Message::NEW_ASSET:
            {
                OnRenderRequestNewObject(sender, msg.GetString(), msg.GetTypeDesc());
            }
            break;
        default:
            break;
    }
}


void AssetIOMessageController::OnRenderRequestOpenObject(const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    if (mApp != nullptr)
    {
        Pegasus::AssetLib::IAssetLibProxy*  assetLib = mApp->GetAssetLibProxy();
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* object = assetLib->LoadObject(asciiPath);
        if (object != nullptr)
        {            
            emit(SignalOpenObject(object));
        }

        emit (SignalUpdateNodeViews());
    }
}

void AssetIOMessageController::OnRenderRequestCloseObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    assetLib->CloseObject(object);
    emit (SignalUpdateNodeViews());
}

void AssetIOMessageController::OnSaveObject(PegasusDockWidget* sender, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::Io::IoError err = mApp->GetAssetLibProxy()->SaveObject(object);

    if (err == Pegasus::Io::ERR_NONE)
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_SAVE_SUCCESS));
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_SAVE_ERROR));
    }
}

void AssetIOMessageController::OnRenderRequestNewObject(PegasusDockWidget* sender, const QString& path, const Pegasus::PegasusAssetTypeDesc* desc)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* newObj = assetLib->CreateObject(asciiPath, desc);
    if (newObj != nullptr)
    {
        if (Pegasus::Io::ERR_NONE == assetLib->SaveObject(newObj))
        {
            emit (SignalUpdateNodeViews());
            emit(SignalOpenObject(newObj));  
        } 
        else
        {
            emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
        }
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
    }
}
