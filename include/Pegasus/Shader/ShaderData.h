/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderData.h
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  ShaderData, represents shader binary data (contains handle)

#ifndef PEGASUS_SHADER_DATA_H
#define PEGASUS_SHADER_DATA_H

#include "Pegasus/Graph/NodeData.h"

namespace Pegasus
{
namespace Shader
{

#if PEGASUS_GAPI_GL
    typedef unsigned int ShaderHandle;
    const ShaderHandle INVALID_SHADER_HANDLE = 0;
#else  
#error "Shader package only supports Open GL"
#endif

//! Class that describes shader data, exposing GLuint handle
class ShaderData : public Pegasus::Graph::NodeData
{
    friend class ShaderStage;
public:

    //! Default Constructor
    //! \param allocator the allocator used for internal allocations
    explicit ShaderData(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~ShaderData();

    //! Returns the GL handle of a shader stage
    //! \return GLuint the GLuint used in glAttachShader opengl calls
    ShaderHandle GetShaderHandle() const { return mHandle; }

private:
    void   SetShaderHandle(ShaderHandle handle){ mHandle = handle; }
    ShaderHandle mHandle;
};

typedef Pegasus::Core::Ref<ShaderData> ShaderDataRef;
typedef Pegasus::Core::Ref<ShaderData> & ShaderDataIn;
typedef Pegasus::Core::Ref<ShaderData> & ShaderDataInOut;
typedef Pegasus::Core::Ref<ShaderData> ShaderDataReturn;


}
}
#endif
