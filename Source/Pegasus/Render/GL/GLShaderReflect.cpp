/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLShaderReflect.cpp

//! \author	Kleber Garcia
//! \date	12 May 2014
//! \brief	Shader reflection information. Cached at shader creation time.
//!         This information is used to determine the attribute locations for quick mesh dispatch

#if PEGASUS_GAPI_GL

#include "../Source/Pegasus/Render/GL/GLShaderReflect.h"

namespace Pegasus
{
namespace Render
{
    void PopulateReflectionInfo(GLuint programHandle, GLShaderReflect& information)
    {
        PG_ASSERTSTR(MESH_MAX_SEMANTIC_INDEX < 10, " if semantic index is greater than 10, this algorithm needs to change!");
        //generate attribute name table
        for (int s = 0; s < Mesh::MeshInputLayout::SEMANTIC_COUNT; ++s)
        {
            for (int i = 0; i < MESH_MAX_SEMANTIC_INDEX; ++i)
            {
                information.mAttributeLocations[s][i] = GL_INVALID_INDEX;
                //if check worth every iteration since this happens at load time and is only 80 entries.
                //this also keeps the codespace small
                if (programHandle != 0) //if valid program handle
                {
                    char attrName[3];
                    attrName[0] = Mesh::MeshInputLayout::SemanticNames[s];
                    attrName[1] = '0' + i;
                    attrName[2] = 0;
                    information.mAttributeLocations[s][i] = glGetAttribLocation(programHandle, attrName);
                }
            }

        }
    }
    
}
}

#endif //Pegasus GL def
