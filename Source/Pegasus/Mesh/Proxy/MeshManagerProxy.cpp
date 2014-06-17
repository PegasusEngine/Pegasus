/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshManagerProxy.cpp
//! \author	Kleber Garcia
//! \date	17th June 2014
//! \brief	Proxy object impl, used by the editor to interact with the mesh manager
#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Mesh/Proxy/MeshManagerProxy.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Mesh/MeshManager.h"

namespace Pegasus
{
namespace Mesh
{

MeshManagerProxy::MeshManagerProxy(MeshManager * object)
: mMeshManager(object)
{
    PG_ASSERT(object != nullptr);
}

void MeshManagerProxy::RegisterEventListener(IMeshEventListener * eventListener)
{
    mMeshManager->RegisterEventListener(eventListener);
}

}
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
