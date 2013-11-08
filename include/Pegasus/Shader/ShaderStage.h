/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderStage.h
//! \author Kleber Garcia
//! \date   15th October 2013
//! \brief  Opengl shading pipeline stage	

#ifndef PEGASUS_SHADER_SHADERSTAGE_H
#define PEGASUS_SHADER_SHADERSTAGE_H

#include "Pegasus/Shader/EventDispatcher.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus {
namespace Shader {

enum ShaderType
{
    VERTEX,
    FRAGMENT,
    TESSELATION_CONTROL,
    TESSELATION_EVALUATION,
    GEOMETRY,
    COMPUTE,
    SHADER_STAGES_COUNT,
    SHADER_STAGE_INVALID,
    SHADER_STAGE_MAX
};

//----------------------------------------------------------------------------------------

class ShaderStage 
#if PEGASUS_SHADER_USE_EDIT_EVENTS
: public EventDispatcher
#endif
{
    friend class Program;
public:
    ShaderStage(Alloc::IAllocator* alloc);
    ~ShaderStage();
        

    bool CompileFromSrc(ShaderType type, const char * src, int stringLength);
    bool CompileFromFile(const char * path, Io::IOManager* loader);
    ShaderType GetStageType() const { return mType; }

private:
    // No copies allowed
    PG_DISABLE_COPY(ShaderStage);

    void DestroyShader();
    bool CompileFromSrcInternal(ShaderType type, const char * src, int stringLength);
    void ProcessErrorLog(const char * errorLog);


    Alloc::IAllocator* mAllocator;
    Pegasus::Io::FileBuffer mFileBuffer;
    ShaderType mType;

    //opengl specifics
    struct OglState
    {
        GLuint mShaderHandle;
        OglState()
        : mShaderHandle(0)
        {
        }
        ~OglState()
        {
        }
    } mOgl;
};


} // namespace Shader
} // namespace Pegasus

#endif // PEGASUS_SHADER_SHADERSTAGE_H
