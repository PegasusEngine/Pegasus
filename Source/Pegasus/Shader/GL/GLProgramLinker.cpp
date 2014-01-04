/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	RenderPlatProgramLinker.cpp
//! \author	Kleber Garcia
//! \date	20th October 2013
//! \brief	Represents an opengl program

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/RenderPlatProgramLinker.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h"

Pegasus::Shader::RenderPlatProgramLinker::RenderPlatProgramLinker()
: mRenderPlatProgramLinkerHandle(0)
{
    for (unsigned int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    {
        mStages[i] = Pegasus::Shader::INVALID_SHADER_HANDLE;
    }
}

Pegasus::Shader::RenderPlatProgramLinker::~RenderPlatProgramLinker()
{
    for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
    { 
        RemoveStage(static_cast<Pegasus::Shader::ShaderType>(i));   
    }

    if (mRenderPlatProgramLinkerHandle != 0)
    {
        glDeleteProgram(static_cast<GLuint>(mRenderPlatProgramLinkerHandle));
    }
}

void Pegasus::Shader::RenderPlatProgramLinker::RemoveStage(Pegasus::Shader::ShaderType stageType)
{
    PG_ASSERTSTR(static_cast<int>(stageType) >= 0
     && stageType <= Pegasus::Shader::SHADER_STAGES_COUNT, "Invalid stage type passed!");
    if (mRenderPlatProgramLinkerHandle != 0 && mStages[static_cast<int>(stageType)] != 0)
    {
        glDetachShader(static_cast<GLuint>(mRenderPlatProgramLinkerHandle), static_cast<GLuint>(mStages[static_cast<int>(stageType)]));
    }
    mStages[static_cast<int>(stageType)] = 0;
}

bool Pegasus::Shader::RenderPlatProgramLinker::Link(GLuint shaderPipeline[Pegasus::Shader::SHADER_STAGES_COUNT])
{
    if (mRenderPlatProgramLinkerHandle == 0)
    {
        mRenderPlatProgramLinkerHandle = static_cast<Pegasus::Shader::ShaderHandle>(glCreateProgram());
    }

    //either have at least a pixel and vertex shader, or have everything null but a single compute shader
    if (
        !(shaderPipeline[Pegasus::Shader::VERTEX] != 0)
        &&  !(
                shaderPipeline[VERTEX] == Pegasus::Shader::INVALID_SHADER_HANDLE &&
                shaderPipeline[FRAGMENT] == Pegasus::Shader::INVALID_SHADER_HANDLE &&
                shaderPipeline[TESSELATION_CONTROL] == Pegasus::Shader::INVALID_SHADER_HANDLE &&
                shaderPipeline[TESSELATION_EVALUATION] == Pegasus::Shader::INVALID_SHADER_HANDLE &&
                shaderPipeline[GEOMETRY] == Pegasus::Shader::INVALID_SHADER_HANDLE &&
                shaderPipeline[COMPUTE] != Pegasus::Shader::INVALID_SHADER_HANDLE
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
            glAttachShader(static_cast<GLuint>(mRenderPlatProgramLinkerHandle), shaderPipeline[i]); 
        }
    }

    glLinkProgram(static_cast<GLuint>(mRenderPlatProgramLinkerHandle)); 
    GLint shaderLinked = GL_FALSE;
    glGetProgramiv(static_cast<GLuint>(mRenderPlatProgramLinkerHandle), GL_LINK_STATUS, &shaderLinked);
    if (shaderLinked == GL_FALSE)
    {
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetProgramInfoLog(static_cast<GLuint>(mRenderPlatProgramLinkerHandle), bufferSize, &logLength, logBuffer);
        SHADEREVENT_LINKING_FAIL(logBuffer);
        return false;
    }

    SHADEREVENT_LINKING_SUCCESS;
    return true;
}
