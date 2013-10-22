/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Program.cpp
//! \author	Kleber Garcia
//! \date	20th October 2013
//! \brief	Represents an opengl program

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/Program.h"

Pegasus::Shader::Program::Program()
: mIsValidProgram(false)
{
    for (int i = 0; i < Pegasus::Shader::SHADER_STAGES_COUNT; ++i)
    {
        mStages[i] = nullptr;
    }
}

Pegasus::Shader::Program::~Program()
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    { 
        RemoveStage(static_cast<Pegasus::Shader::ShaderType>(i));   
    }

    if (mOgl.mProgramHandle != 0)
    {
        glDeleteProgram(mOgl.mProgramHandle);
    }
}

void Pegasus::Shader::Program::SetStage(Pegasus::Shader::ShaderStage * stage)
{
    if (static_cast<int>(stage->GetStageType()) >= 0
     && stage->GetStageType() <= Pegasus::Shader::SHADER_STAGES_COUNT)
    {
        RemoveStage(stage->GetStageType());
        mStages[static_cast<int>(stage->GetStageType())] = stage;
    }
    else
    {
        SHADEREVENT_ATTACHMENT_FAIL(stage);
    }
}

void Pegasus::Shader::Program::RemoveStage(Pegasus::Shader::ShaderType stageType)
{
    PG_ASSERTSTR(static_cast<int>(stageType) >= 0
     && stageType <= Pegasus::Shader::SHADER_STAGES_COUNT, "Invalid stage type passed!");
    if (mOgl.mProgramHandle != 0 && mStages[static_cast<int>(stageType)] != nullptr)
    {
        glDetachShader(mOgl.mProgramHandle, mStages[static_cast<int>(stageType)]->mOgl.mShaderHandle);
        mIsValidProgram = false; //invalidate program
    }
    mStages[static_cast<int>(stageType)] = nullptr;
}

void Pegasus::Shader::Program::Bind() const
{
    if (mOgl.mProgramHandle != 0 && mIsValidProgram)
    {
        glUseProgram(mOgl.mProgramHandle);
    }
    else
    {
        PG_FAILSTR("Trying to bind an invalid shader.");
    }
}

bool Pegasus::Shader::Program::Link()
{
    mIsValidProgram = false;
    if (mOgl.mProgramHandle == 0)
    {
        mOgl.mProgramHandle = glCreateProgram();
    }
    //either have at least a pixel and vertex shader, or have everything null but a single compute shader
    if (
        !(mStages[Pegasus::Shader::VERTEX] != nullptr)
        &&  !(
                mStages[VERTEX] == nullptr &&
                mStages[FRAGMENT] == nullptr &&
                mStages[TESSELATION_CONTROL] == nullptr &&
                mStages[TESSELATION_EVALUATION] == nullptr &&
                mStages[GEOMETRY] == nullptr &&
                mStages[COMPUTE] != nullptr
            )
    )
    {
        SHADEREVENT_LINKING_INCOMPLETE;
        return false;
    }

    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        if (mStages[i] != nullptr)
        {
            glAttachShader(mOgl.mProgramHandle, mStages[i]->mOgl.mShaderHandle); 
        }
    }

    glLinkProgram(mOgl.mProgramHandle); 
    GLint shaderLinked = GL_FALSE;
    glGetProgramiv(mOgl.mProgramHandle, GL_LINK_STATUS, &shaderLinked);
    if (shaderLinked == GL_FALSE)
    {
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetProgramInfoLog(mOgl.mProgramHandle, bufferSize, &logLength, logBuffer);
        SHADEREVENT_LINKING_FAIL(logBuffer);
        return false;
    }

    SHADEREVENT_LINKING_SUCCESS;
    mIsValidProgram = true;
    return true;
}
