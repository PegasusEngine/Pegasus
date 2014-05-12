/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshData.cpp
//! \author	Kleber Garcia
//! \date	01st November 2013
//! \brief	Mesh node data, used by all mesh nodes, including generators and operators

#include "Pegasus/Mesh/MeshData.h"

namespace Pegasus {
namespace Mesh {


MeshData::MeshData(const MeshConfiguration & configuration, Alloc::IAllocator* allocator)
:   Graph::NodeData(allocator),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

MeshData::~MeshData()
{
}


}   // namespace Mesh
}   // namespace Pegasus
