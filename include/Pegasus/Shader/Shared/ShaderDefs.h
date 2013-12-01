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

namespace Pegasus
{
namespace Shader
{

//! Shader Stage Type, mapping to the OpenGl 4.X shader pipeline
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

}
}

#endif //PEGASUS_SHADER_DEFS_H
