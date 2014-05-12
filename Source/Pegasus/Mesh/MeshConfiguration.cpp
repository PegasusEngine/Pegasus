/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshConfiguration.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh configuration used by all mesh nodes, which must be compatible
//!         between nodes to link them

#include "Pegasus/Mesh/MeshConfiguration.h"

namespace Pegasus {
namespace Mesh {


MeshConfiguration::MeshConfiguration()
{
}

//----------------------------------------------------------------------------------------

MeshConfiguration::MeshConfiguration(const MeshConfiguration & other)
{
    *this = other;
}

//----------------------------------------------------------------------------------------

MeshConfiguration & MeshConfiguration::operator=(const MeshConfiguration & other)
{
    return *this;
}


//----------------------------------------------------------------------------------------

bool MeshConfiguration::IsCompatible(const MeshConfiguration & configuration) const
{
    return true;
}


}   // namespace Mesh
}   // namespace Pegasus
