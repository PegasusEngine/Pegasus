/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeneratorNode.cpp
//! \author	Kevin Boulanger
//! \date	19th October 2013
//! \brief	Base generator node class, for all data generators with no input node

#include "Pegasus/Graph/GeneratorNode.h"

namespace Pegasus {
namespace Graph {


GeneratorNode::GeneratorNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Node(nodeAllocator, nodeDataAllocator)
{
}

//----------------------------------------------------------------------------------------

void GeneratorNode::AddInput(NodeIn inputNode)
{
    PG_FAILSTR("Generator nodes are not allowed to have input nodes");
}

//----------------------------------------------------------------------------------------

bool GeneratorNode::Update()
{
    // Since the node is a generator, there is no input node to check.
    // We directly return the state of the node data.
    return AreDataDirty();
}

//----------------------------------------------------------------------------------------

NodeDataReturn GeneratorNode::GetUpdatedData(bool & updated)
{
    // If the data have not been allocated, allocate them now
    if (!AreDataAllocated())
    {
        CreateData();
    }
    PG_ASSERTSTR(AreDataAllocated(), "Node data have to be allocated when being updated");

    // If the data are dirty, re-generate them
    if (AreDataDirty())
    {
        GenerateData();
        GetData()->Validate();
        updated = true;
    }
    PG_ASSERTSTR(!AreDataDirty(), "Node data are supposed to be up-to-date at this point");

    return GetData();
}

//----------------------------------------------------------------------------------------
    
GeneratorNode::~GeneratorNode()
{
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_ASSERT
void GeneratorNode::OnRemoveInput(unsigned int index)
{
    PG_FAILSTR("Generator nodes do not have inputs, so no node is supposed to be removed");
}
#endif  // PEGASUS_ENABLE_ASSERT


}   // namespace Graph
}   // namespace Pegasus
