/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderPlatShaderStage.cpp
//! \author Kleber Garcia
//! \date   17th October 2013
//! \brief  Opengl shading pipeline stage

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/RenderPlatShaderStage.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h"
#include <string.h>


namespace PegasusShaderPrivate {

static struct GLPegasusStageMapping 
{
    Pegasus::Shader::ShaderType mType;
    GLuint mGlRenderPlatShaderStage;
} gGLPegasusStageMappings[Pegasus::Shader::SHADER_STAGES_COUNT] =
{
    { Pegasus::Shader::VERTEX                 , GL_VERTEX_SHADER          },
    { Pegasus::Shader::FRAGMENT               , GL_FRAGMENT_SHADER        },
    { Pegasus::Shader::TESSELATION_CONTROL    , GL_TESS_CONTROL_SHADER    },
    { Pegasus::Shader::TESSELATION_EVALUATION , GL_TESS_EVALUATION_SHADER },
    { Pegasus::Shader::GEOMETRY               , GL_GEOMETRY_SHADER        },
    { Pegasus::Shader::COMPUTE                 , GL_COMPUTE_SHADER         }
}
;

} // namespace PegasusShaderPrivate

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Shader {

RenderPlatShaderStage::RenderPlatShaderStage(Alloc::IAllocator* alloc)
    : mAllocator(alloc), mType(Pegasus::Shader::SHADER_STAGE_INVALID), mShaderHandle(Pegasus::Shader::INVALID_SHADER_HANDLE)
{
}

Pegasus::Shader::RenderPlatShaderStage::~RenderPlatShaderStage()
{
    DestroyShader();
}

void RenderPlatShaderStage::DestroyShader()
{
    if (mShaderHandle != Pegasus::Shader::INVALID_SHADER_HANDLE)
    {
       glDeleteShader(static_cast<GLuint>(mShaderHandle));
       mShaderHandle = Pegasus::Shader::INVALID_SHADER_HANDLE;
    }
    mType = Pegasus::Shader::SHADER_STAGE_INVALID;
}

void RenderPlatShaderStage::ProcessErrorLog(const char * log)
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

bool RenderPlatShaderStage::Compile()
{
    if (mType == Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        //empty shader, ignore compilation
        return false;
    }

    PG_ASSERT(mType >= 0 && mType < Pegasus::Shader::SHADER_STAGES_COUNT);
    
    
    if (mShaderHandle == Pegasus::Shader::INVALID_SHADER_HANDLE)
    {
        mShaderHandle = (Pegasus::Shader::ShaderHandle)glCreateShader(PegasusShaderPrivate::gGLPegasusStageMappings[static_cast<int>(mType)].mGlRenderPlatShaderStage);    
    }
    const GLint stringLength = static_cast<GLuint>(mFileBuffer.GetFileSize());
    const char * shaderSource = mFileBuffer.GetBuffer();
    glShaderSource(
        mShaderHandle,
        1, // count
        &static_cast<const GLchar*>(shaderSource), // src (list of strings, on this case just 1 string
        &stringLength // length of each string in the list of strings,
            //opengl samples use NULL, not nullptr
            // NULL in this argument means just assume each string is a NULL terminated string
    );
    PG_ASSERT(mShaderHandle != Pegasus::Shader::SHADER_STAGE_INVALID);
    glCompileShader(static_cast<GLuint>(mShaderHandle));
    GLint shaderCompiled = GL_TRUE;
    glGetShaderiv(static_cast<GLuint>(mShaderHandle), GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_FALSE)
    {
        //failure shenanigans
        const GLsizei bufferSize = 256;
        char logBuffer[bufferSize];
        GLsizei logLength = 0;
        glGetShaderInfoLog(static_cast<GLuint>(mShaderHandle), bufferSize, &logLength, logBuffer);
        ProcessErrorLog(logBuffer);
    
        SHADEREVENT_COMPILATION_FAIL(logBuffer);

        return false;
    }
    SHADEREVENT_COMPILATION_SUCCESS;
    return true;
}

void RenderPlatShaderStage::SetSource(Pegasus::Shader::ShaderType type, const char * src, int srcSize)
{
    //reallocate buffer size if more space requested on recompilation
    if (srcSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer (
            mAllocator,
            PG_NEW_ARRAY(mAllocator, -1, "shader src", Pegasus::Alloc::PG_MEM_PERM, char, srcSize),
            srcSize
        );
    }
    mFileBuffer.SetFileSize(srcSize);
    memcpy_s(mFileBuffer.GetBuffer(),mFileBuffer.GetBufferSize(),src,srcSize);
    mType = type;
}

bool RenderPlatShaderStage::SetSourceFromFile(Pegasus::Shader::ShaderType type, const char * path, Io::IOManager* loader)
{
    PG_ASSERT(path != nullptr);
    mType = type;
    if (mType != Pegasus::Shader::SHADER_STAGE_INVALID)
    {
        mFileBuffer.DestroyBuffer(); //clear any buffers pre-allocated to this
        Pegasus::Io::IoError ioError = loader->OpenFileToBuffer(path, mFileBuffer, true, mAllocator);
        if (ioError == Pegasus::Io::ERR_NONE)
        {
            SHADEREVENT_LOADED(mFileBuffer.GetBuffer(), mFileBuffer.GetFileSize());
            return true;
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
