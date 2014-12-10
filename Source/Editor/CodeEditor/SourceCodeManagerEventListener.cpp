/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceCodeManagerEventListener.cpp
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Source Code Manager Event Listener	
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"

CodeUserData::CodeUserData(Pegasus::Core::ISourceCodeProxy * code)
: mIsValid(true), mIntermediateDocument(nullptr)
{
    mData.mSourceCode = code;
    mIsProgram = false;
}

CodeUserData::CodeUserData(Pegasus::Shader::IProgramProxy * program)
: mIsValid(true)
{
    mData.mProgram = program;
    mIsProgram = true;
}

SourceCodeManagerEventListener::SourceCodeManagerEventListener (AssetLibraryWidget * widget)
    : QObject(widget), mLibraryWidget(widget)
{
}

SourceCodeManagerEventListener::~SourceCodeManagerEventListener()
{
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::CompilationEvent& e)
{
    if (userData != nullptr)
    {
        CodeUserData * codeUserData = static_cast<CodeUserData*>(userData);
        bool previousVal = codeUserData->IsValid();
        codeUserData->SetIsValid(e.IsSuccess());
        emit( OnCompilationEnd(e.GetLogString()) );
        if (previousVal != e.IsSuccess())
        {
            emit(CompilationResultsChanged());
        }
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::LinkingEvent& e)
{
    if (userData != nullptr)
    {
        CodeUserData * programUserData = static_cast<CodeUserData*>(userData); 
        ED_ASSERT(programUserData->IsProgram());
        bool previousIsValid = programUserData->IsValid();
        bool isSuccess = Pegasus::Core::CompilerEvents::LinkingEvent::LINKING_SUCCESS == e.GetEventType();
        programUserData->SetIsValid(isSuccess);
        programUserData->SetErrorMessage(e.GetLog());
        emit(OnLinkingEvent(e.GetLog(), e.GetEventType()));
        if (previousIsValid != isSuccess)
        {
            emit(CompilationResultsChanged());
        }
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::FileOperationEvent& e)
{
    if (e.GetType() == Pegasus::Core::CompilerEvents::FileOperationEvent::IO_FILE_SAVE_SUCCESS)
    {
        emit(OnSignalSaveSuccess());
    }
    else if (e.GetType() == Pegasus::Core::CompilerEvents::FileOperationEvent::IO_FILE_SAVE_ERROR)
    {
        emit(OnSignalSavedFileError(e.GetIoError(), QString(e.GetMessage())));
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::ObjectOperation& e)
{
    if (e.GetOp() == Pegasus::Core::CompilerEvents::ObjectOperation::CREATED_OPERATION)
    {
        QString objName = e.GetName();
        emit(OnSignalNewObject(objName));
    }
    else if (e.GetOp() == Pegasus::Core::CompilerEvents::ObjectOperation::DESTROYED_OPERATION)
    {
        QString objName = e.GetName();
        emit(OnSignalDestroyObject(userData, objName));
    }
    else
    {
        ED_FAILSTR("unhandled object operation.");
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e)
{
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::CompilationNotification& e)
{
    if (userData != nullptr)
    {
        CodeUserData * codeUserData = static_cast<CodeUserData*>(userData);
        if (e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_BEGIN)
        {
            emit( OnCompilationBegin(codeUserData->GetSourceCode()) ) ;
        }
        else
        {        
            emit(
                OnCompilationError(
                    codeUserData->GetSourceCode(),
                    e.GetRow(),
                    QString(e.GetDescription())
                )
             );
        } 
    }
}
