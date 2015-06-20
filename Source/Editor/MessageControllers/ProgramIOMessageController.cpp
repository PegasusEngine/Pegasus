/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramIOMessageController.cpp
//! \author	Kleber Garcia
//! \date	June 16th 2015
//! \brief	Program IO controller 

#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "MessageControllers/ProgramIOMessageController.h"

ProgramIOMessageController::ProgramIOMessageController(Pegasus::App::IApplicationProxy* app)
: mApp(app)
{
}

ProgramIOMessageController::~ProgramIOMessageController()
{
}

void ProgramIOMessageController::OnRenderThreadProcessMessage(const ProgramIOMessageController::Message& msg)
{
    switch(msg.GetMessageType())
    {
    case ProgramIOMessageController::Message::REMOVE_SHADER:
        OnRenderThreadRemoveShader(msg.GetProgram(), msg.GetShaderType());
        break;
    case ProgramIOMessageController::Message::MODIFY_SHADER:
        OnRenderThreadModifyShader(msg.GetProgram(), msg.GetShaderPath());
        break;
    default:
        break;
    }
}

void ProgramIOMessageController::OnRenderThreadRemoveShader(Pegasus::Shader::IProgramProxy* program, Pegasus::Shader::ShaderType shaderType)
{
    for (int i = 0; i < program->GetShaderCount(); ++i)
    {
        Pegasus::Shader::IShaderProxy* shader = program->GetShader(i);
        if (shader->GetStageType() == shaderType)
        {
            program->RemoveShader(i);
            emit SignalUpdateProgramView();
            break;
        }
    }
}

void ProgramIOMessageController::OnRenderThreadModifyShader(Pegasus::Shader::IProgramProxy* program, const QString& path)
{
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::Shader::IShaderManagerProxy* shaderManagerProxy = mApp->GetShaderManagerProxy();
    Pegasus::AssetLib::IAssetProxy* asset = nullptr;
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();
    if (Pegasus::Io::ERR_NONE == assetLib->LoadAsset(asciiPath, &asset))
    {
        if (shaderManagerProxy->IsShader(asset))
        {
            Pegasus::Shader::IShaderProxy* openedShader = shaderManagerProxy->OpenShader(asset);
            if (openedShader != nullptr)
            {
                program->SetShader(openedShader);                            
                shaderManagerProxy->CloseShader(openedShader); //no need to keep this shader open
                                                               //autoref should take care of it in the program
                emit SignalUpdateProgramView();
            }
        }
        else
        { 
            //todo: error
        }
    }
    else
    {
        //todo: else emit loading error
    }
}

