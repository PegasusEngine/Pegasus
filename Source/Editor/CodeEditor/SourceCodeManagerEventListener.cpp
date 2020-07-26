/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceCodeManagerEventListener.cpp
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Source Code Manager Event Listener	
#include <Application/Application.h>
#include <CodeEditor/SourceCodeManagerEventListener.h>
#include <Pegasus/Shader/Shared/IProgramProxy.h>
#include <Pegasus/AssetLib/Shared/IAssetProxy.h>
#include <QTextDocument>
#include <string.h>

CodeUserData::CodeUserData(Pegasus::Core::ISourceCodeProxy * code)
: mIsValid(true) 
{
    mData.mSourceCode = code;
	mData.mBasicAsset = code;
    mIsProgram = false;
}

CodeUserData::CodeUserData(Pegasus::Shader::IProgramProxy * program)
: mIsValid(true)
{
    mData.mProgram = program;
	mData.mBasicAsset = program;
    mIsProgram = true;
}

SourceCodeManagerEventListener::SourceCodeManagerEventListener ()
    : QObject(nullptr)
{
}

SourceCodeManagerEventListener::~SourceCodeManagerEventListener()
{
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::CompilationEvent& e)
{
    if (userData != nullptr)
    {
        CodeUserData * codeUserData = static_cast<CodeUserData*>(userData);
        bool previousVal = codeUserData->IsValid();
        codeUserData->SetIsValid(e.IsSuccess());
        emit( OnCompilationEnd(e.GetLogString()) );
        if (previousVal != e.IsSuccess())
        {
            QString assetName = tr("[NoAsset]");
            if (codeUserData->IsProgram())
            {
                if (codeUserData->GetProgram()->GetOwnerAsset() != nullptr)
                {
                    assetName = codeUserData->GetProgram()->GetOwnerAsset()->GetPath();
                }
            }
            else
            {
                if (codeUserData->GetSourceCode()->GetOwnerAsset() != nullptr)
                {
                    assetName = codeUserData->GetSourceCode()->GetOwnerAsset()->GetPath();
                }
            }
            emit(OnTagValidity(assetName, e.IsSuccess()));
        }
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::LinkingEvent& e)
{
    ED_ASSERT(userData != nullptr);
    
    CodeUserData * programUserData = static_cast<CodeUserData*>(userData); 
    if (programUserData->GetHandle().IsValid() )
    {
        ED_ASSERT(programUserData->IsProgram());
        bool previousIsValid = programUserData->IsValid();
        bool isSuccess = Pegasus::Core::CompilerEvents::LinkingEvent::LINKING_SUCCESS == e.GetEventType();
        programUserData->SetIsValid(isSuccess);
        emit(OnLinkingEvent(e.GetLog(), e.GetEventType()));
        if (previousIsValid != isSuccess)
        {
            emit(CompilationResultsChanged(isSuccess));
        }
    }
}

void SourceCodeManagerEventListener::OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name)
{
    QString strName = name;
    if (strName == "ProgramLinkage")
    {
        Pegasus::Shader::IProgramProxy* programProxy = static_cast<Pegasus::Shader::IProgramProxy*>(proxy);
        CodeUserData* newUserData = new CodeUserData(programProxy);
        programProxy->SetUserData(newUserData);
    }
    else
    {
        CodeUserData* newUserData = new CodeUserData(static_cast<Pegasus::Core::ISourceCodeProxy*>(proxy));
        proxy->SetUserData(newUserData);
    }
}

void SourceCodeManagerEventListener::OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name)
{
    ED_ASSERT(proxy->GetUserData() != nullptr);
    CodeUserData* userData = static_cast<CodeUserData*>(proxy->GetUserData());
    delete userData;
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e)
{
    //TODO: unused event, this should be removed at some point
}

static AssetInstanceHandle GetCodeHandle(CodeUserData* userData)
{
    if (userData->IsProgram())
    {
		Pegasus::Shader::IProgramProxy* program = userData->GetProgram();
        if (!program)
            return AssetInstanceHandle();

        Pegasus::Core::ISourceCodeProxy* codeProxy = program->GetSourceCode();
        if (!codeProxy)
            return AssetInstanceHandle();

        CodeUserData* codeUserData = static_cast<CodeUserData*>(codeProxy->GetUserData());
        return codeUserData->GetHandle();
    }
    else
    {
        return userData->GetHandle();
    }
}

void SourceCodeManagerEventListener::OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::CompilationNotification& e)
{
    ED_ASSERT(userData != nullptr);
    CodeUserData * codeUserData = static_cast<CodeUserData*>(userData);
    if (codeUserData->GetHandle().IsValid())
    {
		AssetInstanceHandle childHandle = GetCodeHandle(codeUserData);
		auto* assetProxy = codeUserData->GetBasicAsset()->GetOwnerAsset();

        if (e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_BEGIN)
        {
            emit( OnCompilationBegin(childHandle) ) ;
        }
        else if (e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_ERROR || e.GetType() == Pegasus::Core::CompilerEvents::CompilationNotification::COMPILATION_WARNING)
        {
			if (assetProxy)
			{
				QString fullError = tr("%1 >> %2").arg(tr(assetProxy->GetPath()), tr(e.GetDescription()));
				Application::LogHandler('ERR_', fullError.toLocal8Bit().constData());
			}

            //notify the code editor:
            emit(
                OnCompilationError(
					childHandle,
                    e.GetRow(),
                    QString(e.GetDescription())
                )
             );
        } 
    }
}
