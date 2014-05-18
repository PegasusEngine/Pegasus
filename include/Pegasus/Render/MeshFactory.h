/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MeshFactory.h
//! \author Kleber Garcia
//! \date   16th May 2014
//! \brief  Factory creator functions. These function pointers will return factories for
//!         the node classes to be used in the conversion of GPU to CPU data
#ifndef PEGASUS_RENDER_MESH_FACTORY_H
#define PEGASUS_RENDER_MESH_FACTORY_H

#include "Pegasus/Mesh/IMeshFactory.h"

namespace Pegasus {
namespace Render {

//! \return returns a mesh factory implementation.
//!         this implementation needs to be passed to the MeshManager
Pegasus::Mesh::IMeshFactory * GetRenderMeshFactory();

}
}

#endif
