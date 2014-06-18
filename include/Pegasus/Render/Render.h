/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Render.h
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  Master render library. PARR - Pegasus Abstract Render Recipes	
//!         This library encapsulates most of the underlying API and serves as a way 
//!         of accessing it. Provides render shortcuts.

#ifndef PARR_H
#define PARR_H

#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Texture/Texture.h"

#define PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN 64 

namespace Pegasus
{

namespace Render
{

    //! Structure representing a uniform location in a particular shader
    //! The name holds a copy of the actual uniform name.
    //! The internal values are obfuscated, not to be used.
    struct Uniform
    {
        char mName[PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN];
        int  mInternalIndex;
        int  mInternalOwner;
        int  mInternalVersion;
    public:
            Uniform()
            : mInternalIndex(-1), mInternalVersion(-1), mInternalOwner(-1)
            {
                mName[0] = 0;
            }
    };

    //! Structure representing a buffer location in the GPU
    //! the size holds the size of this buffer at creation time
    //! the mInternalData is data used by the specific graphis library
    //! In openGL this would be used as buffer usage and the index of the buffer
    //! in D3D this would be the raw device child pointer
    struct Buffer
    {
        int mSize;
        unsigned long long mInternalData; //! 64 bit compressed data, used internally
    public:
        Buffer()
           : mInternalData(0), mSize(0)
        {
        }
    };

    //! Dispatches a shader.
    //! \param program the shader program to dispatch
    void Dispatch (Shader::ProgramLinkageInOut program);

    //! Dispatches a mesh.
    //! \param mesh that the system will dispatch.
    //! \WARNING: a shader must have been dispatched before this call.
    //!           not dispatching a shader will cause the mesh to be incorrectly rendered
    //!           or throw an assert
    void Dispatch (Mesh::MeshInOut mesh);

    //! Draws geometry.
    //! \note Requires: -Shader to be dispatched
    //!                 -Mesh to be dispatched
    void Draw();

    //! Fills a uniform reference by name
    //! \param program, the program containing the uniform to get
    //! \param outputUniform, the empty uniform structure to be filled containing the metadata required
    //! \param name, the constant name of the uniform that we will be trying to find in the uniform table
    //! \return boolean, true if the uniform was found, false otherwise
    bool GetUniformLocation(Shader::ProgramLinkageInOut program, const char * name, Uniform& outputUniform);

    //! Creates a buffer optimized for uniform usage
    //! \param bufferSize, the size of the buffer to be used in bytes
    //! \param outputBuffer the output struct to be filled
    void CreateUniformBuffer(int bufferSize, Buffer& outputBuffer);

    //! Memcpys a buffer to the gpu destination.
    //! \param dstBuffer the GPU destination buffer to copy to
    //! \param src the source buffer to use
    //! \param size, the size of the src buffer to copy to dstBuffer in bytes. If -1, 
    //!        it will use the size registered in dstBuffer
    //! \param offset, the offset to use
    void SetBuffer(Buffer& dstBuffer, void * src, int size = -1, int offset = 0);


    //! Deletes a buffer created from CreateUniformBuffer
    //! \param outputBuffer the buffer to delete
    void DeleteBuffer(Buffer& outputBuffer);

    //! Sets the uniform value
    //! \param u uniform to set the value to
    //! \param value the actual value set
    //! \return boolean, true on success, false on error
    bool SetUniform(Uniform& u, float value);

    //! Sets the uniform value to a texture
    //! \param u uniform to set the value to
    //! \param texture to set to the uniform slot
    //! \return boolean, true on success, false on error
    bool SetUniform(Uniform& u, Texture::TextureInOut texture);

    //! Sets the uniform block buffer
    //! \param u uniform block to set the value to
    //! \param buffer to set to the uniform block slot
    //! \return boolean, true on success, false on error
    bool SetUniform(Uniform& u, const Buffer& buffer);


}

}

#endif
