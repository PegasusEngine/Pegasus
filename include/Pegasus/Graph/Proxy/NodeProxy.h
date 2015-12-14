/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeProxy.h
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy object, used by the editor to interact with a graph node

#ifndef PEGASUS_GRAPH_PROXY_NODEPROXY_H
#define PEGASUS_GRAPH_PROXY_NODEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Graph/Shared/INodeProxy.h"

namespace Pegasus {
    namespace Graph {
        class Node;
    }
}

namespace Pegasus {
namespace Graph {


//! Proxy object, used by the editor to interact with a graph node
class NodeProxy : public INodeProxy
{
public:

    //! Constructor
    //! \param node Node associated with the proxy (!= nullptr)
    NodeProxy(const Node* node);

    //! Destructor
    virtual ~NodeProxy();

    //! Get the node associated with the proxy
    //! \return Node associated with the proxy (!= nullptr)
    inline const Node* GetNode() const { return mNode; }


    //! \todo Accessors

    //------------------------------------------------------------------------------------

private:

    //! Proxied graph node object
    const Node* const mNode;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_GRAPH_PROXY_NODEPROXY_H
