/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IMeshManagerProxy.h
//! \author	Kleber Garcia
//! \date	17th June 2014
//! \brief	Proxy object interface, used by the editor to interact with the mesh manager

#ifndef PEGASUS_IMESHMANAGERPROXY_H
#define PEGASUS_IMESHMANAGERPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace Mesh
{

class IMeshEventListener;

//! Proxy interface, used by the editor to interact with the meshes
class IMeshManagerProxy
{
public:
    
    //! Destructor
    virtual ~IMeshManagerProxy() {}

    //! Sets the event listener to be used for the mesh manager
    //! \param event listener reference
    virtual void RegisterEventListener(IMeshEventListener * eventListener) = 0;
};


}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif // PEGASUS_MESHMANAGERPROXY_H
