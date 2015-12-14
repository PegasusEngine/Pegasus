/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	INodeInputProxy.h
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy interface, used by the editor to interact with an input of a graph node

#ifndef PEGASUS_GRAPH_SHARED_INODEINPUTPROXY_H
#define PEGASUS_GRAPH_SHARED_INODEINPUTPROXY_H

#if PEGASUS_ENABLE_PROXIES


namespace Pegasus {
namespace Graph {

class INodeProxy;


//! Proxy interface, used by the editor to interact with an input of a graph node
class INodeInputProxy
{
public:

    //! Destructor
    virtual ~INodeInputProxy() { }


    //! Get the proxy of the node owning this input
    //! \return Proxy of the node owning this input (!= nullptr)
    virtual const INodeProxy* GetNodeProxy() const = 0;

    //! Get the name of the input
    //! \return Name of the input (non-empty string)
    virtual const char* GetName() const = 0;

    //! Test if the input is required to be connected
    //! \return True if the input is required to be connected
    virtual bool IsRequired() const = 0;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_GRAPH_SHARED_INODEINPUTPROXY_H
