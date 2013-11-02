/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	OperatorNode.cpp
//! \author	Kevin Boulanger
//! \date	23rd October 2013
//! \brief	Base operator node class, for all operators with at least one input

#include "Pegasus/Graph/OperatorNode.h"

namespace Pegasus {
namespace Graph {


OperatorNode::OperatorNode()
:   Node()
{
}

//----------------------------------------------------------------------------------------

void OperatorNode::AddInput(NodeIn inputNode)
{
    Node::AddInput(inputNode);
}

//----------------------------------------------------------------------------------------

NodeDataReturn OperatorNode::GetUpdatedData(bool & updated)
{
    // Check the number of inputs
    const unsigned int minNumInputs = GetMinNumInputNodes();
    const unsigned int maxNumInputs = GetMaxNumInputNodes();
    const unsigned int numInputs = GetNumInputs();
    if ((numInputs < minNumInputs) || (numInputs > maxNumInputs))
    {
        PG_FAILSTR("Invalid number of inputs for a node (%d), it should be between %d and %d",
                   numInputs, minNumInputs, maxNumInputs);
        return GetData();
    }

    // If the data have not been allocated, allocate them now
    if (!AreDataAllocated())
    {
        CreateData();
    }
    PG_ASSERTSTR(AreDataAllocated(), "Node data have to be allocated when being updated");

    // Get the updated data for every input
    bool inputUpdated = false;
    for (unsigned int i = 0; i < numInputs; ++i)
    {
        NodeDataRef updatedData = GetUpdatedData(inputUpdated);
    }

    // If any input has been updated or if the data are dirty, re-generate them
    if (inputUpdated || AreDataDirty())
    {
        GenerateData();
        GetData()->Validate();
        updated = true;
    }
    PG_ASSERTSTR(!AreDataDirty(), "Node data are supposed to be up-to-date at this point");

    return GetData();
}

//----------------------------------------------------------------------------------------

//virtual void ReleaseDataAndPropagate();

//----------------------------------------------------------------------------------------
    
OperatorNode::~OperatorNode()
{
}

//----------------------------------------------------------------------------------------

void OperatorNode::OnRemoveInput(unsigned int index)
{
}


}   // namespace Graph
}   // namespace Pegasus
