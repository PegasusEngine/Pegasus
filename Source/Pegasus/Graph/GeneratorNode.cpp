/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeneratorNode.cpp
//! \author	Karolyn Boulanger
//! \date	19th October 2013
//! \brief	Base generator node class, for all data generators with no input node

#include "Pegasus/Graph/GeneratorNode.h"

namespace Pegasus {
namespace Graph {


GeneratorNode::GeneratorNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Node(nodeAllocator, nodeDataAllocator)
{
#if PEGASUS_ENABLE_PROXIES
    mNodeType = NODETYPE_GENERATOR;
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

bool GeneratorNode::Update()
{
    if (IsDataAllocated() && GetPropertyGrid().IsDirty())
    {
        // If the property grid has members that are updated, invalidate the data
        GetData()->Invalidate();
    }

    // Validate the property grid to track any subsequent changes
    GetPropertyGrid().Validate();

    // Since the node is a generator, there is no input node to check.
    // We directly return the state of the node data.
    return IsDataDirty();
}

//----------------------------------------------------------------------------------------

NodeDataReturn GeneratorNode::GetUpdatedData(bool & updated)
{
    // If the data has not been allocated, allocate it now
    if (!IsDataAllocated())
    {
        CreateData();
    }
    PG_ASSERTSTR(IsDataAllocated(), "Node data has to be allocated when being updated");

    // If the data is dirty, re-generate it
    if (IsDataDirty())
    {
        // No need to re-invalidate the GPU data, it is automatically invalidated
        // when the node data is invalidated

        // Generate the node data using the generator-specific code
        GenerateData();

        // Validate the node data, the GPU node data is still dirty
        GetData()->Validate();

        updated = true;
    }
    PG_ASSERTSTR(!IsDataDirty(), "Node data is supposed to be up-to-date at this point");

    return GetData();
}

//----------------------------------------------------------------------------------------
    
GeneratorNode::~GeneratorNode()
{
}

//----------------------------------------------------------------------------------------

void GeneratorNode::AddInput(NodeIn inputNode)
{
    PG_FAILSTR("Generator nodes are not allowed to have input nodes");
}

//----------------------------------------------------------------------------------------

void GeneratorNode::ReplaceInput(unsigned int index, const Pegasus::Core::Ref<Node> & inputNode)
{
    PG_FAILSTR("Generator nodes are not allowed to have input nodes");
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
