#include "MessageControllers/AssetIOMessageController.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
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
                OnRenderRequestOpenAsset(msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::CLOSE_CODE:
            {
                OnRenderRequestCloseCode(msg.GetAssetNode().mCode);
            }
            break;
        case AssetIOMessageController::Message::SAVE_PROGRAM:
        case AssetIOMessageController::Message::SAVE_TIMELINE:
        case AssetIOMessageController::Message::SAVE_CODE:
            {
                OnSaveObject(sender, msg.GetAssetNode().mCode);
            }
            break;
        case AssetIOMessageController::Message::CLOSE_PROGRAM:
            {
                OnRenderRequestCloseProgram(msg.GetAssetNode().mProgram);
            }
            break;
        case AssetIOMessageController::Message::NEW_SHADER:
            {
                OnRenderRequestNewShader(sender, msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::NEW_TIMELINESCRIPT:
            {
                OnRenderRequestNewTimelineScript(sender, msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::NEW_PROGRAM:
            {
                OnRenderRequestNewProgram(sender, msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::NEW_TEXTURE:
        case AssetIOMessageController::Message::NEW_MESH:
        default:
            break;
    }
}


void AssetIOMessageController::OnRenderRequestCloseCode(Pegasus::Core::ISourceCodeProxy* code)
{
    CodeUserData* userData = static_cast<CodeUserData*>(code->GetUserData());
    if (userData->GetName() == "ShaderStage")
    {
        Pegasus::Shader::IShaderManagerProxy* shaderMgrProxy = mApp->GetShaderManagerProxy();
        shaderMgrProxy->CloseShader(static_cast<Pegasus::Shader::IShaderProxy*>(userData->GetSourceCode()));
        emit(SignalUpdateNodeViews());
    }
    else if(userData->GetName() == "BlockScript")
    {
        Pegasus::Timeline::ITimelineManagerProxy* timelineProxy = mApp->GetTimelineManagerProxy();
        timelineProxy->CloseScript(userData->GetSourceCode());
        emit(SignalUpdateNodeViews());
    }
    else
    {
        ED_FAILSTR("Invalid source code to close!!");
    }
}

void AssetIOMessageController::OnRenderRequestOpenAsset(const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    if (mApp != nullptr)
    {
        Pegasus::AssetLib::IAssetLibProxy*     assetLib = mApp->GetAssetLibProxy();
        Pegasus::Shader::IShaderManagerProxy*  shaderManagerProxy  = mApp->GetShaderManagerProxy();
        Pegasus::Timeline::ITimelineManagerProxy*     timelineProxy = mApp->GetTimelineManagerProxy();
        Pegasus::AssetLib::IAssetProxy*        asset = nullptr;
        Pegasus::Io::IoError errCode = assetLib->LoadAsset(asciiPath, &asset);
        if (Pegasus::Io::ERR_NONE == errCode)
        {
            ED_ASSERT(asset != nullptr);

            if (shaderManagerProxy->IsShader(asset))
            {
                Pegasus::Shader::IShaderProxy* openedShader = shaderManagerProxy->OpenShader(asset);
                if (openedShader != nullptr)
                {
                    emit(SignalOpenCode(openedShader));
                }
            } 
            else if (shaderManagerProxy->IsProgram(asset))
            {
                Pegasus::Shader::IProgramProxy* programProxy = shaderManagerProxy->OpenProgram(asset);
                if (programProxy != nullptr)
                {
                    emit(SignalOpenProgram(programProxy));
                }
            }
            else if (timelineProxy->IsTimelineScript(asset))
            {
                Pegasus::Core::ISourceCodeProxy* openedTimelineScript = timelineProxy->OpenScript(asset);
                if (openedTimelineScript != nullptr)
                {
                    emit (SignalOpenCode(openedTimelineScript));
                }
            }

            emit (SignalUpdateNodeViews());
        }
        else
        {
            SignalOnErrorMessagePopup(tr("IO Error: Cannot load asset. Pegasus::Io Error code: %1").arg(static_cast<int>(errCode)));
        }
    }
}

void AssetIOMessageController::OnRenderRequestCloseProgram(Pegasus::Shader::IProgramProxy* program)
{
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::Shader::IShaderManagerProxy* shaderManagerProxy = mApp->GetShaderManagerProxy();
    shaderManagerProxy->CloseProgram(program);
    emit (SignalUpdateNodeViews());
}

void AssetIOMessageController::OnSaveObject(PegasusDockWidget* sender, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::AssetLib::IAssetProxy* ass = object->GetOwnerAsset();
    ED_ASSERTSTR(ass != nullptr, "Saving asset: cannot be an invalid value!");
    object->Write(ass);
    Pegasus::Io::IoError err = mApp->GetAssetLibProxy()->SaveAsset(ass);

    if (err == Pegasus::Io::ERR_NONE)
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_SAVE_SUCCESS));
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_SAVE_ERROR));
    }
}

void AssetIOMessageController::OnRenderRequestNewShader(PegasusDockWidget* sender, const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IAssetProxy* asset = nullptr;
    Pegasus::Io::IoError errCode = assetLib->CreateBlankAsset(asciiPath, &asset);
    if (errCode == Pegasus::Io::ERR_NONE)
    {
        Pegasus::Shader::IShaderManagerProxy* shaderMgr = mApp->GetShaderManagerProxy();
        Pegasus::Shader::IShaderProxy* shader = shaderMgr->OpenShader(asset);
        if (shader != nullptr)
        {
            emit(SignalOpenCode(shader));
        }
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
    }
}

void AssetIOMessageController::OnRenderRequestNewTimelineScript(PegasusDockWidget* sender, const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IAssetProxy* asset = nullptr;
    Pegasus::Io::IoError errCode = assetLib->CreateBlankAsset(asciiPath, &asset);
    if (errCode == Pegasus::Io::ERR_NONE)
    {
        Pegasus::Timeline::ITimelineManagerProxy* timelineMgr = mApp->GetTimelineManagerProxy();
        Pegasus::Core::ISourceCodeProxy* code = timelineMgr->OpenScript(asset);
        if (code != nullptr)
        {
            emit(SignalOpenCode(code));
        }
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
    }
}

void AssetIOMessageController::OnRenderRequestNewProgram(PegasusDockWidget* sender, const QString& path)
{
    
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IAssetProxy* asset = nullptr;
    Pegasus::Io::IoError errCode = assetLib->CreateBlankAsset(asciiPath, &asset);
    if (errCode == Pegasus::Io::ERR_NONE)
    {
        int indexOfDot = path.lastIndexOf('.') - 1;
        int indexOfSlash = path.lastIndexOf('/');
        int indexOfBackSlash = path.lastIndexOf('\\');

        int beginIndex = (indexOfSlash > indexOfBackSlash ? indexOfSlash : indexOfBackSlash);
        int endIndex = indexOfDot <= path.length() - 1 ? indexOfDot : path.length() - 1;
        QString programName = tr("unnamed");
        programName = path.mid(beginIndex + 1, endIndex - beginIndex);

        QByteArray ba = programName.toLocal8Bit();
        Pegasus::Shader::IProgramProxy* program = mApp->GetShaderManagerProxy()->CreateNewProgram(ba.constData());
        program->Write(asset);
        program->Bind(asset);
        errCode = assetLib->SaveAsset(asset);
        if (errCode != Pegasus::Io::ERR_NONE)
        {
            emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
        }
        else if (program != nullptr)
        {
            emit(SignalOpenProgram(program));
        }
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMessageController::Message::IO_NEW_ERROR));
    }
}
