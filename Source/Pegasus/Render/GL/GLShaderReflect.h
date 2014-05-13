/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLShaderReflect.h

//! \author	Kleber Garcia
//! \date	12 May 2014
//! \brief	Shader reflection information. Cached at shader creation time.
//!         This information is used to determine the attribute locations for quick mesh dispatch

#ifndef PEGASUS_RENDER_SHADERREFLECT_H
#define PEGASUS_RENDER_SHADERREFLECT_H

#if PEGASUS_GAPI_GL

#include "Pegasus/Mesh/MeshInputLayout.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus
{
namespace Render
{

//! structure that contains the cached attribute location for this shader
//! fascilitates to two memory accesses to gather the proper attribute location (without the use of strings)
struct GLShaderReflect
{
    GLuint mAttributeLocations[Mesh::MeshInputLayout::SEMANTIC_COUNT][MESH_MAX_SEMANTIC_INDEX];
};

//! Populates the attribute table based on the attribute description, defined in the string table of MeshInputLayout
//! \param programHandle the program handle to extract known attribute data from.
//! \param information the reflection information struct to fill
void PopulateReflectionInfo(GLuint programHandle, GLShaderReflect& information);

}
}

#endif //Pegasus GL def
#endif //Pegasus shader reflect
