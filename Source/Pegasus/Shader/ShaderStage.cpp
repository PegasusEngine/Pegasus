/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderStage.cpp
//! \author Kleber Garcia
//! \date   17th October 2013
//! \brief  Opengl shading pipeline stage

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/ShaderStage.h"
#include <string.h>
#undef ERROR

namespace PegasusShaderPrivate {

static struct ShaderStageProperties
{
    Pegasus::Shader::ShaderType mType;
    const char * mExtension;
    GLuint mGlShaderStage;
} gShaderStageProperties[Pegasus::Shader::SHADER_STAGES_COUNT] =
{
    { Pegasus::Shader::VERTEX,                 ".vs" , GL_VERTEX_SHADER          },
    { Pegasus::Shader::FRAGMENT,               ".ps" , GL_FRAGMENT_SHADER        },
    { Pegasus::Shader::TESSELATION_CONTROL,    ".tcs", GL_TESS_CONTROL_SHADER    },
    { Pegasus::Shader::TESSELATION_EVALUATION, ".tes", GL_TESS_EVALUATION_SHADER },
    { Pegasus::Shader::GEOMETRY,               ".gs" , GL_GEOMETRY_SHADER        },
    { Pegasus::Shader::COMPUTE,                ".cs" , GL_COMPUTE_SHADER         }
}
;

} // namespace PegasusShaderPrivate

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Shader {

ShaderStage::ShaderStage()
 : mType(Pegasus::Shader::SHADER_STAGE_INVALID)
{
}

Pegasus::Shader::ShaderStage::~ShaderStage()
{
    DestroyShader();
}

void ShaderStage::DestroyShader()
{
    if (mOgl.mShaderHandle)
    {
       glDeleteShader(mOgl.mShaderHandle);
    }
    mType = Pegasus::Shader::SHADER_STAGE_INVALID;
}

void ShaderStage::ProcessErrorLog(const char * log)
{
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    //parsing log to extract line & column
    const char  * s = log;
 
    const int descriptionBufferSize = 256;
    char descriptionBuffer[descriptionBufferSize];
    char * descriptionBufferLimit = descriptionBuffer + descriptionBufferSize;

    //WARNING, I have optimized the crap out of the following code, so parsing might be messy.
    // I am trying to parse the following string:
    // ERROR {row:column}: {Description}
    // OpenGl logs throw errors in this format.

    while (*s)
    {
        bool isErrorStr = true;

        //find "ERROR" substring
        const char * errorStr = "ERROR";
        for (; *errorStr && isErrorStr; ++errorStr) isErrorStr = *errorStr == *(s++);

        if (isErrorStr)
        {
            int line = 0;
            int column = 0;
        
            //Skip spaces and colons
            while (*s == ' ' || *s == ':') ++s;

            //Parse Line number
            for (;*s >= '0' && *s <= '9'; ++s) line = 10*line + (*s - '0');

            //Skip spaces and colons
            while (*s == ' ' || *s == ':') ++s; // skip

            //Parse Column number
            for (;*s >= '0' && *s <= '9'; ++s) column = 10*column + (*s - '0');

            //Skip spaces and colons
            while (*s == ' ' || *s == ':') ++s; // skip

            //cache description buffer
            char * descPtr = descriptionBuffer;
            for (;*s && descPtr != descriptionBufferLimit && *s != '\n';++s) *(descPtr++) = *s;
            PG_ASSERTSTR(descPtr !=descriptionBufferLimit, "DANGER!! buffer overflow!");
            *descPtr = '\0';

            SHADEREVENT_COMPILATION_ERROR(line, column, descriptionBuffer);
        }
    }
#endif
}

bool ShaderStage::CompileFromSrcInternal(Pegasus::Shader::ShaderType type, const char * src, int bufferLength)
{
    mType = type;
    PG_ASSERT(type >= 0 && type < Pegasus::Shader::SHADER_STAGES_COUNT);
    
    if (mOgl.mShaderHandle == 0)
    {
        mOgl.mShaderHandle = glCreateShader(PegasusShaderPrivate::gShaderStageProperties[static_cast<int>(type)].mGlShaderStage);    
    }
    const GLint stringLength = static_cast<GLuint>(bufferLength);
    glShaderSource(
        mOgl.mShaderHandle,
        1, // count
        &static_cast<const GLchar*>(src), // src (list of strings, on this case just 1 string
        &stringLength // length of each string in the list of strings,
            //opengl samples use NULL, not nullptr
            // NULL in this argument means just assume each string is a NULL terminated string
    );
    PG_ASSERT(mOgl.mShaderHandle != 0);
    glCompileShader(mOgl.mShaderHandle);
    GLint shaderCompiled = GL_TRUE;
    glGetShaderiv(mOgl.mShaderHandle, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_FALSE)
    {
        //failure shenanigans
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetShaderInfoLog(mOgl.mShaderHandle, bufferSize, &logLength, logBuffer);
        ProcessErrorLog(logBuffer);
    
        SHADEREVENT_COMPILATION_FAIL(logBuffer);

        return false;
    }
    SHADEREVENT_COMPILATION_SUCCESS;
    return true;
}

bool ShaderStage::CompileFromSrc(Pegasus::Shader::ShaderType type, const char * src, int srcSize)
{
    //reallocate buffer size if more space requested on recompilation
    if (srcSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer (
            PG_NEW("shader src", Pegasus::Memory::PG_MEM_PERM) char[srcSize],
            srcSize
        );
        mFileBuffer.SetFileSize(srcSize);
        memcpy_s(mFileBuffer.GetBuffer(),mFileBuffer.GetBufferSize(),src,srcSize);
    }
    return CompileFromSrcInternal(type, mFileBuffer.GetBuffer(), mFileBuffer.GetFileSize());
}

bool ShaderStage::CompileFromFile(const char * path, Io::IOManager* loader)
{
    PG_ASSERT(path != nullptr);
    const char * extension = strrchr(path, '.');
    Pegasus::Shader::ShaderType targetStage = Pegasus::Shader::SHADER_STAGE_INVALID;
    if (extension != nullptr)
    {
        for (int i = 0; i < static_cast<int>(Pegasus::Shader::SHADER_STAGES_COUNT); ++i)
        {
            if (!_stricmp(extension, PegasusShaderPrivate::gShaderStageProperties[i].mExtension))
            {
                targetStage = PegasusShaderPrivate::gShaderStageProperties[i].mType;
                break;
            }
        }
    }

    if (targetStage != Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        mFileBuffer.DestroyBuffer(); //clear any buffers pre-allocated to this
        Pegasus::Io::IoError ioError = loader->OpenFileToBuffer(path, mFileBuffer, true);
        if (ioError == Pegasus::Io::ERR_NONE)
        {
            SHADEREVENT_LOADED(mFileBuffer.GetBuffer(), mFileBuffer.GetFileSize());
            return CompileFromSrcInternal(targetStage, mFileBuffer.GetBuffer(), mFileBuffer.GetFileSize());
        }
        else
        {
            SHADEREVENT_IO_ERROR(ioError, path, "Io error");
        }
    }
    else
    {
        SHADEREVENT_WRONG_FILE_FORMAT(path, "wrong file format!");
    }
    return false;
}


}  // namespace Shader
}  // namespace Pegasus
