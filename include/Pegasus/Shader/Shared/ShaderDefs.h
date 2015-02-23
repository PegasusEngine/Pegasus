/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderDefs.h
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  General enums / shared definition in the shader package
#ifndef PEGASUS_SHADER_DEFS_H
#define PEGASUS_SHADER_DEFS_H

//!forward declarations

namespace Pegasus
{
namespace Io
{
    class IOManager;
}
}

namespace Pegasus
{
namespace Shader
{
    class IEventListener;
    class IUserData;

//! Shader Stage Type, mapping to the OpenGL 4.X shader pipeline
enum ShaderType
{
    VERTEX, // vertex shader stage
    FRAGMENT, // pixel shader stage
    TESSELATION_CONTROL, // tesselation control stage
    TESSELATION_EVALUATION, // tesselation evaluation stage
    GEOMETRY, // geometry shader stage
    COMPUTE, // compute shader stage. Must be by itself if activated
    SHADER_STAGES_COUNT, // count, use this to iterate through stages
    SHADER_STAGE_INVALID, // invalid flag, meaning shader is not set
    SHADER_STAGE_MAX // maximum count for stage enumeration
};

//! Shader stage configuration structure
struct ShaderStageProperties
{
    Pegasus::Shader::ShaderType mType; //! type for shader stage
    const char * mSource; //! the source string
    int          mSourceSize; //! the source string precomputed size
        
    ShaderStageProperties()
    :
    mType(Pegasus::Shader::SHADER_STAGE_INVALID),
    mSource(nullptr),
    mSourceSize(0)
    {
    }
};

}
}

#endif //PEGASUS_SHADER_DEFS_H
