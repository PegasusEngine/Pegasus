/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshManagerProxy.cpp
//! \author	Kleber Garcia
//! \date	17th June 2014
//! \brief	Proxy object impl, used by the editor to interact with the mesh manager

#ifndef PEGASUS_MESHMANAGER_PROXY_H
#define PEGASUS_MESHMANAGER_PROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Mesh/Shared/IMeshManagerProxy.h"

namespace Pegasus
{
namespace Mesh
{

class IMeshEventListener;
class MeshManager;

//! Proxy interface, used by the editor to interact with the meshes
class MeshManagerProxy : public IMeshManagerProxy
{
public:
    
    //! Constructor
    explicit MeshManagerProxy(MeshManager * object);

    //! Destructor
    virtual ~MeshManagerProxy() {}

    //! Sets the event listener to be used for the mesh manager
    //! \param event listener reference
    virtual void RegisterEventListener(IMeshEventListener * eventListener);

private:
    MeshManager * const  mMeshManager;
};


}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif //PEGASUS_MESHMANAGERPROXY_H
