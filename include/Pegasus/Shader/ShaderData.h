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
#include "Pegasus/Render/GL/GLEWStaticInclude.h"

namespace Pegasus
{
namespace Shader
{
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
    GLuint GetGlHandle() const { return mHandle; }

private:
    void   SetGlHandle(GLuint handle){ mHandle = handle; }
    GLuint mHandle;
};

typedef Pegasus::Core::Ref<ShaderData> ShaderDataRef;
typedef Pegasus::Core::Ref<ShaderData> & ShaderDataIn;
typedef Pegasus::Core::Ref<ShaderData> & ShaderDataInOut;
typedef Pegasus::Core::Ref<ShaderData> ShaderDataReturn;


}
}
#endif
