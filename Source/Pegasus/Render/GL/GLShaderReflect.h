/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLShaderReflect.h

//! \author	Kleber Garcia
//! \date	12 May 2014
//! \brief	Shader reflection information. Cached at shader creation time.
//!         This information is used to determine the attribute and uniform locations for quick mesh dispatch

#ifndef PEGASUS_RENDER_SHADERREFLECT_H
#define PEGASUS_RENDER_SHADERREFLECT_H

#if PEGASUS_GAPI_GL

#include "Pegasus/Render/Render.h"
#include "Pegasus/Mesh/MeshInputLayout.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace Render
{

//! opengl uniform entry.
//! holds all the metadata of a uniform
struct GLShaderUniform
{
    char mName[PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN];
    GLuint mSlot;
    GLenum mType;
    GLint  mTextureSlot;
    GLint  mUniformBlockSize;

    GLShaderUniform()
    {
        mSlot = GL_INVALID_INDEX;
        mType = GL_ZERO;
        mName[0] = 0;
        mUniformBlockSize = -1;
    }
    
};

//! structure that contains the cached attribute location for this shader
//! fascilitates to two memory accesses to gather the proper attribute location (without the use of strings)
struct GLShaderReflect
{
    GLuint mAttributeLocations[Mesh::MeshInputLayout::SEMANTIC_COUNT][MESH_MAX_SEMANTIC_INDEX]; //! table of semantic / semantic index attributes

    //! table holding the cached uniform from shader reflection data
    struct UniformTable
    {
        GLShaderUniform * mTable;
        int mTableSize;
        int mTableCount;
        UniformTable()
            : mTable(nullptr), mTableSize(0), mTableCount(0)
        {
        }
    } mUniformTable;
};

//! Populates the attribute table based on the attribute description, defined in the string table of MeshInputLayout
//! \param programHandle the program handle to extract known attribute data from.
//! \param information the reflection information struct to fill
//! \param triggerAlignmentWarning output variable. True if there is an alignment issue
void PopulateReflectionInfo(
    Pegasus::Alloc::IAllocator * alloc,
    GLuint programHandle, 
    GLShaderReflect& information,
    bool& triggerAlignmentWarning 
);

//! Destroys shader reflection info
//! \param alloc the allocator used originally
//! \param the shader reflection information structure
void DestroyReflectionInfo(Pegasus::Alloc::IAllocator * alloc, GLShaderReflect& information);

}
}

#endif //Pegasus GL def
#endif //Pegasus shader reflect
