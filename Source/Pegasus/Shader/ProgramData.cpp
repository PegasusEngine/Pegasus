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
    PG_ASSERT(GetGlHandle() != 0);
    glUseProgram(GetGlHandle());
}
