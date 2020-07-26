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
#include "CodeEditor\SourceCodeManagerEventListener.h"
#include "Pegasus/PegasusAssetTypes.h"

ProgramIOMessageController::ProgramIOMessageController(Pegasus::App::IApplicationProxy* app)
: mApp(app)
{
}

ProgramIOMessageController::~ProgramIOMessageController()
{
}

void ProgramIOMessageController::OnRenderThreadProcessMessage(const ProgramIOMCMessage& msg)
{
    switch(msg.GetMessageType())
    {
    case ProgramIOMCMessage::REMOVE_SHADER:
        OnRenderThreadRemoveShader(msg.GetProgram());
        break;
    case ProgramIOMCMessage::MODIFY_SHADER:
        OnRenderThreadModifyShader(msg.GetProgram(), msg.GetShaderPath());
        break;
    default:
        break;
    }
}

void ProgramIOMessageController::OnRenderThreadModifyShader(AssetInstanceHandle handle, const QString& path)
{
    Pegasus::Shader::IProgramProxy* program = static_cast<Pegasus::Shader::IProgramProxy*>(FindInstance(handle));

    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();
    bool isNew = false;
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* object = assetLib->LoadObject(asciiPath, &isNew);

    if ( object != nullptr )
    {
        auto* openedShader = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
        program->SetSourceCode(openedShader);                            
        
        if (isNew) //only close if this shader was never open to avoid destroying memory
        {
            assetLib->CloseObject(object); //no need to keep this shader open
        }
        emit SignalUpdateProgramView();
        emit SignalRedrawViewports();
    }
    else
    {
        //todo: else emit loading error
    }
}

void ProgramIOMessageController::OnRenderThreadRemoveShader(AssetInstanceHandle handle)
{
    Pegasus::Shader::IProgramProxy* program = static_cast<Pegasus::Shader::IProgramProxy*>(FindInstance(handle));
    program->SetSourceCode(nullptr);

    emit SignalUpdateProgramView();
    emit SignalRedrawViewports();
}

QVariant ProgramIOMessageController::TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::Shader::IProgramProxy* program = static_cast<Pegasus::Shader::IProgramProxy*>(object);

    ED_ASSERT(program->GetUserData() != nullptr);

    CodeUserData* codeUserData = static_cast<CodeUserData*>(program->GetUserData());

    //! the user data now obtains the handle. This will help the event listener to
    //! send messages back to the widgets
    codeUserData->SetHandle(handle);

    QVariantMap programInfo;
    Pegasus::Core::ISourceCodeProxy* shaderSource = program->GetSourceCode(); 
    if (shaderSource != nullptr)
    {
        QVariantMap shaderObject;
        shaderObject.insert("Path", shaderSource->GetOwnerAsset()->GetPath());
        programInfo.insert("Shader", shaderObject);
    }

    programInfo.insert("Name", program->GetName());
    return programInfo;
}

const Pegasus::PegasusAssetTypeDesc** ProgramIOMessageController::GetTypeList() const 
{
    static const Pegasus::PegasusAssetTypeDesc* gTypes[] = {
             &Pegasus::ASSET_TYPE_PROGRAM  
            ,nullptr
    };
    
    return gTypes;
}
