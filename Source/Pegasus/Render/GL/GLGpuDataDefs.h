/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLGpuDataDefs.h
//! \author	Kleber Garcia
//! \date	23rd March 2014
//! \brief	Basic declaration of custom opengl gpu data. Each gpu data type has its own RTTI
//!         safety declaration
//!         WARNING!! do not declare constructor on custom gpu data. initialize data in respective
//!                   factory functions

#ifndef PEGASUS_RENDER_GLGPUDEFS_H
#define PEGASUS_RENDER_GLGPUDEFS_H

#if PEGASUS_GAPI_GL

#include "Pegasus/Graph/NodeGpuData.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "../Source/Pegasus/Render/GL/GLShaderReflect.h"

namespace Pegasus
{
namespace Render
{

//! basic internal container class with opengl handles for shaders
struct OglShaderGpuData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OglShaderGpuData, 0x1);
    GLuint mHandle; //! open gl handle
};

//! basic internal container class with opengl handles for programs
struct OglProgramGpuData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OglProgramGpuData, 0x2);
    GLuint mHandle; //! open gl handle
    GLuint mShaderCachedHandles [Pegasus::Shader::SHADER_STAGES_COUNT]; //! open gl cached handles for binded shaders
    GLShaderReflect mReflection; //! reflection data
};

//! basic internal container class with opengl handles for textures
struct OglTextureGpuData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OglTextureGpuData, 0x3);
};

//! basic internal container class with opengl handles for meshes
struct OglMeshGpuData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OglMeshGpuData, 0x4);
};

} // namespace Render
} // namespace Pegasus

#endif // PEGASUS_GAPI_GL

#endif //include guard
