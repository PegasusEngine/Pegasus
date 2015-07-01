#include "MessageControllers/AssetIOMessageController.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
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

void AssetIOMessageController::OnRenderThreadProcessMessage(const AssetIOMessageController::Message& msg)
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
        case AssetIOMessageController::Message::SAVE_CODE:
            {
                OnRenderRequestSaveCode(msg.GetAssetNode().mCode);
            }
            break;
        case AssetIOMessageController::Message::CLOSE_PROGRAM:
            {
                OnRenderRequestCloseProgram(msg.GetAssetNode().mProgram);
            }
            break;
        case AssetIOMessageController::Message::SAVE_PROGRAM:
            {
                OnRenderRequestSaveProgram(msg.GetAssetNode().mProgram);
            }
            break;
        case AssetIOMessageController::Message::NEW_SHADER:
            {
                OnRenderRequestNewShader(msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::NEW_TIMELINESCRIPT:
            {
                OnRenderRequestNewTimelineScript(msg.GetString());
            }
            break;
        case AssetIOMessageController::Message::NEW_PROGRAM:
            {
                OnRenderRequestNewProgram(msg.GetString());
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
        Pegasus::Timeline::ITimelineProxy* timelineProxy = mApp->GetTimelineProxy();
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
        Pegasus::Timeline::ITimelineProxy*     timelineProxy = mApp->GetTimelineProxy();
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

void AssetIOMessageController::OnRenderRequestSaveCode(Pegasus::Core::ISourceCodeProxy* code)
{
    if (mApp == nullptr) return;
    CodeUserData* codeUserData = static_cast<CodeUserData*>(code->GetUserData());
    Pegasus::Io::IoError err = Pegasus::Io::ERR_NONE;
    if (codeUserData->GetName() == "ShaderStage")
    {
        Pegasus::Shader::IShaderManagerProxy*  shaderManagerProxy  = mApp->GetShaderManagerProxy();
        Pegasus::AssetLib::IAssetProxy* ass = shaderManagerProxy->GetShaderAsset(static_cast<Pegasus::Shader::IShaderProxy*>(code));
        ED_ASSERTSTR(ass != nullptr, "Saving asset: cannot be an invalid value!");

        shaderManagerProxy->FlushShaderToAsset(static_cast<Pegasus::Shader::IShaderProxy*>(code));
        err = mApp->GetAssetLibProxy()->SaveAsset(ass);
    }
    else if (codeUserData->GetName() == "BlockScript")
    {
        Pegasus::Timeline::ITimelineProxy*  timelineManager  = mApp->GetTimelineProxy();
        Pegasus::AssetLib::IAssetProxy* ass = timelineManager->GetScriptAsset(code);
        ED_ASSERTSTR(ass != nullptr, "Saving asset: cannot be an invalid value!");

        timelineManager->FlushScriptToAsset(code);
        err = mApp->GetAssetLibProxy()->SaveAsset(ass);
    }

    if (err == Pegasus::Io::ERR_NONE)
    {
        emit(SignalPostCodeMessage(tr("Saved file successfully.")));
    }
    else
    {
        emit(SignalPostCodeMessage(tr("IO Error saving file.")));
    }
}

void AssetIOMessageController::OnRenderRequestSaveProgram(Pegasus::Shader::IProgramProxy* program)
{
    if (mApp == nullptr) return;
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::Shader::IShaderManagerProxy* shaderManagerProxy = mApp->GetShaderManagerProxy();
    shaderManagerProxy->FlushProgramToAsset(program);
    Pegasus::AssetLib::IAssetProxy* ass = shaderManagerProxy->GetProgramAsset(program);
    ED_ASSERTSTR(ass != nullptr, "There must be an asset associated with this!");
    Pegasus::Io::IoError err = assetLib->SaveAsset(ass);
    if (err == Pegasus::Io::ERR_NONE)
    {
        emit(SignalPostProgramMessage(tr("Saved file successfully")));
    }
    else
    {
        emit(SignalPostProgramMessage(tr("IO Error saving file.")));
    }
}

void AssetIOMessageController::OnRenderRequestNewShader(const QString& path)
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
        emit(SignalPostCodeMessage(tr("IO Error creating new shader file.")));
    }
}

void AssetIOMessageController::OnRenderRequestNewTimelineScript(const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IAssetProxy* asset = nullptr;
    Pegasus::Io::IoError errCode = assetLib->CreateBlankAsset(asciiPath, &asset);
    if (errCode == Pegasus::Io::ERR_NONE)
    {
        Pegasus::Timeline::ITimelineProxy* timelineMgr = mApp->GetTimelineProxy();
        Pegasus::Core::ISourceCodeProxy* code = timelineMgr->OpenScript(asset);
        if (code != nullptr)
        {
            emit(SignalOpenCode(code));
        }
    }
    else
    {
        emit(SignalPostCodeMessage(tr("IO Error creating new timeline file.")));
    }
}

void AssetIOMessageController::OnRenderRequestNewProgram(const QString& path)
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

        int beginIndex = indexOfSlash > indexOfBackSlash ? indexOfSlash : indexOfBackSlash;
        beginIndex = beginIndex < 0 ? 0 : beginIndex;
        int endIndex = indexOfDot <= path.length() - 1 ? indexOfDot : path.length() - 1;
        QString programName = tr("unnamed");
        programName = path.mid(beginIndex, endIndex - beginIndex + 1);

        Pegasus::Shader::IShaderManagerProxy* shaderMgr = mApp->GetShaderManagerProxy();
        QByteArray ba = programName.toLocal8Bit();
        Pegasus::Shader::IProgramProxy* program = shaderMgr->CreateNewProgram(ba.constData());
        shaderMgr->BindProgramToAsset(program, asset);
        shaderMgr->FlushProgramToAsset(program);
        errCode = assetLib->SaveAsset(asset);
        if (errCode != Pegasus::Io::ERR_NONE)
        {
            emit(SignalPostProgramMessage(tr("IO Error creating program file")));
        }
        else if (program != nullptr)
        {
            emit(SignalOpenProgram(program));
        }
    }
    else
    {
        emit(SignalPostProgramMessage(tr("IO Error creating program file")));
    }
}
