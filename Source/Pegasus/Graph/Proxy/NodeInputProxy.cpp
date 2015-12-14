/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeInputProxy.cpp
//! \author	Karolyn Boulanger
//! \date	08th December 2015
//! \brief	Proxy object, used by the editor to interact with an input of a graph node

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Graph/Proxy/NodeInputProxy.h"
#include "Pegasus/Graph/NodeInput.h"
#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


NodeInputProxy::NodeInputProxy(const NodeInput* nodeInput)
:   mNodeInput(nodeInput)
{
    PG_ASSERT(nodeInput != nullptr);
}

//----------------------------------------------------------------------------------------

NodeInputProxy::~NodeInputProxy()
{
}

//----------------------------------------------------------------------------------------

const INodeProxy* NodeInputProxy::GetNodeProxy() const
{
    return mNodeInput->GetNode()->GetProxy();
}

//----------------------------------------------------------------------------------------

const char* NodeInputProxy::GetName() const
{
    return mNodeInput->GetName();
}

//----------------------------------------------------------------------------------------

bool NodeInputProxy::IsRequired() const
{
    return mNodeInput->IsRequired();
}


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
