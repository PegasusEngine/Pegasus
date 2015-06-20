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
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"


SourceIOMessageController::SourceIOMessageController(Pegasus::App::IApplicationProxy* proxy)
: mApp(proxy)
{
}

SourceIOMessageController::~SourceIOMessageController()
{
}

void SourceIOMessageController::OnRenderThreadProcessMessage(const SourceIOMessageController::Message& msg)
{
    switch(msg.GetMessageType())
    {
    case SourceIOMessageController::Message::COMPILE_SOURCE:
        OnRenderRequestCompileSource(msg.GetSource());
        break;
    case SourceIOMessageController::Message::SET_SOURCE:
        OnRenderRequestSetSource(msg.GetSource(), msg.GetSourceText());
        break;
    case SourceIOMessageController::Message::SET_SOURCE_AND_COMPILE_SOURCE:
        OnRenderRequestSetSource(msg.GetSource(), msg.GetSourceText());
        OnRenderRequestCompileSource(msg.GetSource());
    default:
        break;
    }
}

void SourceIOMessageController::OnRenderRequestCompileSource(Pegasus::Core::ISourceCodeProxy* src)
{
    src->Compile();
    emit(SignalCompilationRequestEnded());
    emit(SignalRedrawViewports());
}

void SourceIOMessageController::OnRenderRequestSetSource(Pegasus::Core::ISourceCodeProxy* src, const QString& srcTxt)
{
    QByteArray arr = srcTxt.toLocal8Bit();
    const char * source = arr.data();
    int sourceSize = arr.size();
    src->SetSource(source, sourceSize);
}
