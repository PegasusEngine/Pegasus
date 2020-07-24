/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceIOMessageController.cpp
//! \author Kleber Garcia
//! \date   June 6th 2015
//! \brief  Source IO Controller, organized class that executes commands towards the render thread
//!         these commands intend to modify shaders.

#include "MessageControllers/SourceIOMessageController.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"


SourceIOMessageController::SourceIOMessageController(Pegasus::App::IApplicationProxy* proxy)
: mApp(proxy)
{
}

SourceIOMessageController::~SourceIOMessageController()
{
}

void SourceIOMessageController::OnRenderThreadProcessMessage(const SourceIOMCMessage& msg)
{
    switch(msg.GetMessageType())
    {
    case SourceIOMCMessage::COMPILE_SOURCE:
        OnRenderRequestCompileSource(msg.GetHandle());
        break;
    case SourceIOMCMessage::SET_SOURCE:
        OnRenderRequestSetSource(msg.GetHandle(), msg.GetSourceText());
        break;
    case SourceIOMCMessage::SET_SOURCE_AND_COMPILE_SOURCE:
        OnRenderRequestSetSource(msg.GetHandle(), msg.GetSourceText());
        OnRenderRequestCompileSource(msg.GetHandle());
    default:
        break;
    }
}

void SourceIOMessageController::OnRenderRequestCompileSource(AssetInstanceHandle handle)
{
    ED_ASSERT(handle.IsValid());
    Pegasus::Core::ISourceCodeProxy* src = static_cast<Pegasus::Core::ISourceCodeProxy*>(FindInstance(handle));
    src->Compile();
    emit(SignalCompilationRequestEnded());
    emit(SignalRedrawViewports());
}

void SourceIOMessageController::OnRenderRequestSetSource(AssetInstanceHandle handle, const QString& srcTxt)
{
    ED_ASSERT(handle.IsValid());
    Pegasus::Core::ISourceCodeProxy* src = static_cast<Pegasus::Core::ISourceCodeProxy*>(FindInstance(handle));
    QByteArray arr = srcTxt.toLocal8Bit();
    const char * source = arr.data();
    int sourceSize = arr.size();
    src->SetSource(source, sourceSize);
}

QVariant SourceIOMessageController::TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    ED_ASSERT(handle.IsValid());
    Pegasus::Core::ISourceCodeProxy* codeProxy = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
    ED_ASSERT(codeProxy->GetUserData() != nullptr);

    CodeUserData* codeUserData = static_cast<CodeUserData*>(codeProxy->GetUserData());

    //! the user data now obtains the handle. This will help the event listener to
    //! send messages back to the widgets
    codeUserData->SetHandle(handle);

    int srcSize = 0;
    const char* src = nullptr;
    codeProxy->GetSource(&src, srcSize);

    QChar * qchar = new QChar[srcSize];
    for (int i = 0; i < srcSize; ++i)
    {
        qchar[i] = src[i];
    }

    QString srcQString(qchar, srcSize);
    delete[] qchar;            

    QVariantMap vm;
    vm.insert("Source", srcQString);
    vm.insert("CompilationPolicy", static_cast<int>(codeProxy->GetCompilationPolicy()));
    return vm;
}

bool SourceIOMessageController::IsRuntimeObjectValid(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::Core::ISourceCodeProxy* codeProxy = static_cast<Pegasus::Core::ISourceCodeProxy*>(object);
    ED_ASSERT(codeProxy->GetUserData() != nullptr);

    CodeUserData* codeUserData = static_cast<CodeUserData*>(codeProxy->GetUserData());
    return codeUserData->IsValid();

}

const Pegasus::PegasusAssetTypeDesc** SourceIOMessageController::GetTypeList() const
{
    static const Pegasus::PegasusAssetTypeDesc* gTypes[] = {
             &Pegasus::ASSET_TYPE_SHADER_SRC    
            ,&Pegasus::ASSET_TYPE_BLOCKSCRIPT  
            ,&Pegasus::ASSET_TYPE_H_BLOCKSCRIPT
            ,nullptr
    };

    return gTypes;
}
