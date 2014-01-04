/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderData.cpp
//! \author Kleber Garcia
//! \date   1st December 2013
//! \brief  Implementation of ShaderData class

#include "Pegasus/Shader/ShaderData.h"



Pegasus::Shader::ShaderData::ShaderData(Alloc::IAllocator * allocator)
:
Pegasus::Graph::NodeData(allocator),
mHandle(0)
{
    
}

Pegasus::Shader::ShaderData::~ShaderData()
{
}
