/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeInputProxy.h
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy object, used by the editor to interact with an input of a graph node

#ifndef PEGASUS_GRAPH_PROXY_NODEINPUTPROXY_H
#define PEGASUS_GRAPH_PROXY_NODEINPUTPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Graph/Shared/INodeInputProxy.h"

namespace Pegasus {
    namespace Graph {
        class NodeInput;
    }
}

namespace Pegasus {
namespace Graph {


//! Proxy object, used by the editor to interact with an input of a graph node
class NodeInputProxy : public INodeInputProxy
{
public:

    //! Constructor
    //! \param nodeInput Node input associated with the proxy (!= nullptr)
    NodeInputProxy(const NodeInput* nodeInput);

    //! Destructor
    virtual ~NodeInputProxy();

    //! Get the node input associated with the proxy
    //! \return Node input associated with the proxy (!= nullptr)
    inline const NodeInput* GetNodeInput() const { return mNodeInput; }


    //! Get the proxy of the node owning this input
    //! \return Proxy of the node owning this input (!= nullptr)
    virtual const INodeProxy* GetNodeProxy() const override;

    //! Get the name of the input
    //! \return Name of the input (non-empty string)
    virtual const char* GetName() const override;

    //! Test if the input is required to be connected
    //! \return True if the input is required to be connected
    virtual bool IsRequired() const override;

    //------------------------------------------------------------------------------------

private:

    //! Proxied graph node input object
    const NodeInput* const mNodeInput;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_GRAPH_PROXY_NODEINPUTPROXY_H
