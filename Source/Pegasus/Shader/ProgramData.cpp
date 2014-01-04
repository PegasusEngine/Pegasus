/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ProgramData.cpp
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Implementation of ProgramData class

#include "Pegasus/Shader/ProgramData.h"

#if PEGASUS_GAPI_GL
#include "Pegasus/Render/GL/GLEWStaticInclude.h"
#endif

Pegasus::Shader::ProgramData::ProgramData(Alloc::IAllocator* allocator)
:
Pegasus::Graph::NodeData(allocator), mHandle(0)
{
}

Pegasus::Shader::ProgramData::~ProgramData()
{
}

void Pegasus::Shader::ProgramData::Use() const
{
#if PEGASUS_GAPI_GL
    //! TODO come up with a nicer way to implement this convinience function.
    //       we want to avoid platform specific code all over these handles
    PG_ASSERT(GetHandle() != Pegasus::Shader::INVALID_SHADER_HANDLE);
    glUseProgram(static_cast<GLuint>(GetHandle()));
#else
#error "Other graphics API not supported in this package yet"
#endif
}
