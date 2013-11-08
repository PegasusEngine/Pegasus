/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	OutputNode.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base output node class, for the root of the graphs

#include "Pegasus/Graph/OutputNode.h"

namespace Pegasus {
namespace Graph {


OutputNode::OutputNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Node(nodeAllocator, nodeDataAllocator)
{
}

//----------------------------------------------------------------------------------------

void OutputNode::AddInput(NodeIn inputNode)
{
    // Only one input node accepted
    if (GetNumInputs() != 0)
    {
        PG_FAILSTR("Output nodes are not allowed to have multiple input nodes");
        return;
    }

    Node::AddInput(inputNode);
}

//----------------------------------------------------------------------------------------

bool OutputNode::Update()
{
    // Check that no data is allocated
    PG_ASSERTSTR(!AreDataAllocated(), "Invalid output node, it should not contain NodeData");

    // Check that the input node is defined
    if (GetNumInputs() == 1)
    {
        // Update the input node and return its dirty state
        return GetInput(0)->Update();
    }
    else
    {
        // If no input is found, we consider there is no node data to update
        PG_FAILSTR("Invalid output node, it does not have an input defined");
        return false;
    }
}

//----------------------------------------------------------------------------------------
    
NodeDataReturn OutputNode::GetUpdatedData(bool & updated)
{
    // Check that no data is allocated
    PG_ASSERTSTR(!AreDataAllocated(), "Invalid output node, it should not contain NodeData");

    // Check that the input node is defined
    if (GetNumInputs() == 1)
    {
        // Redirect the updated data from the input node
        return GetInput(0)->GetUpdatedData(updated);
    }
    else
    {
        PG_FAILSTR("Invalid output node, it does not have an input defined");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------
    
OutputNode::~OutputNode()
{
}

//----------------------------------------------------------------------------------------

NodeData * OutputNode::AllocateData() const
{
    PG_FAILSTR("Output nodes do not have data, so there is nothing to allocate");
    return nullptr;
}

//----------------------------------------------------------------------------------------

void OutputNode::GenerateData()
{
    PG_FAILSTR("Output nodes do not have data, so there is nothing to generate");
}

//----------------------------------------------------------------------------------------

void OutputNode::OnRemoveInput(unsigned int index)
{
}


}   // namespace Graph
}   // namespace Pegasus
