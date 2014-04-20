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
: mProgram(program)
{
}

ShaderManagerEventListener::ShaderManagerEventListener (ShaderLibraryWidget * widget)
    : QObject(widget), mLibraryWidget(widget)
{
}

ShaderManagerEventListener::~ShaderManagerEventListener()
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::CompilationEvent& e)
{
    if (userData != nullptr)
    {
        ShaderUserData * shaderUserData = static_cast<ShaderUserData*>(userData);
        bool previousIsValid = shaderUserData->IsValid();
        shaderUserData->SetIsValid(e.IsSuccess());
        if (previousIsValid != e.IsSuccess())
        {
            emit(CompilationResultsChanged());
        }
    }
}

void ShaderManagerEventListener::OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::LinkingEvent& e)
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::FileOperationEvent& e)
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::ShaderLoadedEvent& e)
{
}

void ShaderManagerEventListener::OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::CompilationNotification& e)
{
}
