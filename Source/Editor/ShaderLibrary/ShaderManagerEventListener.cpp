/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderManagerEventListener.cpp
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Shader Manager Event Listener	
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include "ShaderLibrary/ShaderEditorWidget.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"

ShaderUserData::ShaderUserData(Pegasus::Shader::IShaderProxy * shader)
: mShader(shader), mIsValid(true)
{
}

ProgramUserData::ProgramUserData(Pegasus::Shader::IProgramProxy * program)
: mProgram(program), mIsValid(true)
{
}

ShaderManagerEventListener::ShaderManagerEventListener (ShaderLibraryWidget * widget)
    : QObject(widget), mLibraryWidget(widget)
{
}

ShaderManagerEventListener::~ShaderManagerEventListener()
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::CompilationEvent& e)
{
    if (userData != nullptr)
    {
        ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(userData);
        bool previousIsValid = shaderUserData->IsValid();
        shaderUserData->SetIsValid(e.IsSuccess());
        if (previousIsValid != e.IsSuccess())
        {
            emit(CompilationResultsChanged(shaderUserData->GetShader()));
        }
    }
}

void ShaderManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::LinkingEvent& e)
{
    if (userData != nullptr)
    {
        ProgramUserData * programUserData = static_cast<ProgramUserData*>(userData); 
        emit(OnLinkingEvent(programUserData->GetProgram(), e.GetLog(), e.GetEventType()));
    }
}

void ShaderManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::FileOperationEvent& e)
{
    //error loading a shader file
    ED_ASSERTSTR("Shader file not found: %s", e.GetMessage());
}

void ShaderManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::ShaderLoadedEvent& e)
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::CompilationNotification& e)
{
    if (userData != nullptr)
    {
        ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(userData);
        if (e.GetType() == Pegasus::Shader::CompilationNotification::COMPILATION_BEGIN)
        {
            emit( OnCompilationBegin(shaderUserData->GetShader()) ) ;
        }
        else
        {        
            emit(
                shaderUserData->GetShader(),
                OnCompilationError(
                    shaderUserData->GetShader(),
                    e.GetRow(),
                    QString(e.GetDescription())
                )
             );
        } 
    }
}
