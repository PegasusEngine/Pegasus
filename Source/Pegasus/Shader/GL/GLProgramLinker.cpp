/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLProgramLinker.cpp
//! \author	Kleber Garcia
//! \date	20th October 2013
//! \brief	Represents an opengl program

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/GL/GLProgramLinker.h"

Pegasus::Shader::GLProgramLinker::GLProgramLinker()
: mGLProgramLinkerHandle(0)
{
    for (unsigned int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        mStages[i] = 0;
    }
}

Pegasus::Shader::GLProgramLinker::~GLProgramLinker()
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    { 
        RemoveStage(static_cast<Pegasus::Shader::ShaderType>(i));   
    }

    if (mGLProgramLinkerHandle != 0)
    {
        glDeleteProgram(mGLProgramLinkerHandle);
    }
}

void Pegasus::Shader::GLProgramLinker::RemoveStage(Pegasus::Shader::ShaderType stageType)
{
    PG_ASSERTSTR(static_cast<int>(stageType) >= 0
     && stageType <= Pegasus::Shader::SHADER_STAGES_COUNT, "Invalid stage type passed!");
    if (mGLProgramLinkerHandle != 0 && mStages[static_cast<int>(stageType)] != 0)
    {
        glDetachShader(mGLProgramLinkerHandle, mStages[static_cast<int>(stageType)]);
    }
    mStages[static_cast<int>(stageType)] = 0;
}

bool Pegasus::Shader::GLProgramLinker::Link(GLuint shaderPipeline[Pegasus::Shader::SHADER_STAGES_COUNT])
{
    if (mGLProgramLinkerHandle == 0)
    {
        mGLProgramLinkerHandle = glCreateProgram();
    }

    //either have at least a pixel and vertex shader, or have everything null but a single compute shader
    if (
        !(shaderPipeline[Pegasus::Shader::VERTEX] != 0)
        &&  !(
                shaderPipeline[VERTEX] == 0 &&
                shaderPipeline[FRAGMENT] == 0 &&
                shaderPipeline[TESSELATION_CONTROL] == 0 &&
                shaderPipeline[TESSELATION_EVALUATION] == 0 &&
                shaderPipeline[GEOMETRY] == 0 &&
                shaderPipeline[COMPUTE] != 0
            )
    )
    {
        SHADEREVENT_LINKING_INCOMPLETE;
        return false;
    }


    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        RemoveStage(static_cast<Pegasus::Shader::ShaderType>(i));
        if (shaderPipeline[i] != 0)
        {
            mStages[i] = shaderPipeline[i];
            glAttachShader(mGLProgramLinkerHandle, shaderPipeline[i]); 
        }
    }

    glLinkProgram(mGLProgramLinkerHandle); 
    GLint shaderLinked = GL_FALSE;
    glGetProgramiv(mGLProgramLinkerHandle, GL_LINK_STATUS, &shaderLinked);
    if (shaderLinked == GL_FALSE)
    {
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetProgramInfoLog(mGLProgramLinkerHandle, bufferSize, &logLength, logBuffer);
        SHADEREVENT_LINKING_FAIL(logBuffer);
        return false;
    }

    SHADEREVENT_LINKING_SUCCESS;
    return true;
}
