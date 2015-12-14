/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeProxy.cpp
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy object, used by the editor to interact with a graph node

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Graph/Proxy/NodeProxy.h"
#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


NodeProxy::NodeProxy(const Node* node)
:   mNode(node)
{
    PG_ASSERT(node != nullptr);
}

//----------------------------------------------------------------------------------------

NodeProxy::~NodeProxy()
{
}


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
