/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	INodeProxy.h
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy interface, used by the editor to interact with a graph node

#ifndef PEGASUS_GRAPH_SHARED_INODEPROXY_H
#define PEGASUS_GRAPH_SHARED_INODEPROXY_H

#if PEGASUS_ENABLE_PROXIES


namespace Pegasus {
namespace Graph {


//! Proxy interface, used by the editor to interact with a graph node
class INodeProxy
{
public:

    //! Destructor
    virtual ~INodeProxy() { }


    //! \todo Accessors
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_GRAPH_SHARED_INODEPROXY_H
