/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GLGPUDataDefs.h
//! \author	Kleber Garcia
//! \date	23rd March 2014
//! \brief	Basic declaration of custom OpenGL GPU data. Each GPU data type has its own RTTI
//!         safety declaration
//!         WARNING!! do not declare constructor on custom GPU data. initialize data in respective
//!                   factory functions

#ifndef PEGASUS_RENDER_GLGPUDEFS_H
#define PEGASUS_RENDER_GLGPUDEFS_H

#if PEGASUS_GAPI_GL

#include "Pegasus/Graph/NodeGPUData.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Mesh/MeshInputLayout.h"
#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "../Source/Pegasus/Render/GL/GLShaderReflect.h"

namespace Pegasus
{
namespace Render
{

//! basic internal container class with OpenGL handles for shaders
struct OGLShaderGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OGLShaderGPUData, 0x1);
    GLuint mHandle; //! OpenGL handle
};

//! basic internal container class with OpenGL handles for programs
struct OGLProgramGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OGLProgramGPUData, 0x2);
    GLuint mHandle; //! OpenGL handle
    GLuint mShaderCachedHandles [Pegasus::Shader::SHADER_STAGES_COUNT]; //! OpenGL cached handles for binded shaders
    GLShaderReflect mReflection; //! reflection data
    short mGUID; //! GUID, every program must have a unique ID
    short mVersion; //! Used to keep track of the compilation counts. Helps other metadata to be updated when a shader
                    //! has changed.
};

//! basic internal container class with OpenGL handles for textures
struct OGLTextureGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OGLTextureGPUData, 0x3);
    GLuint mHandle;
};

//! basic internal container class with OpenGL handles for meshes
struct OGLMeshGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OGLMeshGPUData, 0x4);

    struct DrawState {
        bool mIsIndexed;
        int  mIndexCount;
        int  mVertexCount;
        GLuint mPrimitive;
    } mDrawState;

    struct VAOEntry {
        GLuint mVAOName;
        short mProgramGUID;
        short mProgramVersion;
        VAOEntry()
        {
            mVAOName = GL_INVALID_INDEX;
            mProgramGUID = -1;
            mProgramVersion = -1;
        }
    } * mVAOTable;  //! cached VAO table, one per shader

    int mVAOTableSize; //! total table size
    int mVAOTableCount;

    GLuint mBufferTable[MESH_MAX_STREAMS];
    GLuint mIndexBuffer;
    
};

//! basic internal container class with OpenGL handles for render targets (super set of textures)
struct OGLRenderTargetGPUData
{
    PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(OGLRenderTargetGPUData, 0x5);
    OGLTextureGPUData mTextureView;
    GLuint            mFrameBufferName;
};

} // namespace Render
} // namespace Pegasus

#endif // PEGASUS_GAPI_GL

#endif //include guard
