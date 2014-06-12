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
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace Mesh {


MeshConfiguration::MeshConfiguration()
    :
mIsIndexed(true),
mIsDynamic(false),
mPrimitiveType(TRIANGLE)
{
}

//----------------------------------------------------------------------------------------

MeshConfiguration::MeshConfiguration(const MeshConfiguration & other)
{
    Pegasus::Utils::Memcpy(this, &other, sizeof(MeshConfiguration));
}

//----------------------------------------------------------------------------------------

MeshConfiguration & MeshConfiguration::operator=(const MeshConfiguration & other)
{
    Pegasus::Utils::Memcpy(this, &other, sizeof(MeshConfiguration));
    return *this;
}


}   // namespace Mesh
}   // namespace Pegasus
