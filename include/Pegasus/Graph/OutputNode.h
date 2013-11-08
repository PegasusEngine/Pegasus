/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	OutputNode.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Base output node class, for the root of the graphs

#ifndef PEGASUS_GRAPH_OUTPUTNODE_H
#define PEGASUS_GRAPH_OUTPUTNODE_H

#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class OutputNode : public Node
{
public:

    //! Default constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    OutputNode(Memory::IAllocator * nodeAllocator, Memory::IAllocator * nodeDataAllocator);

    //! Append a node to the list of input nodes
    //! \param inputNode Node to add to the list of input nodes (equivalent to NodeIn)
    //! \warning Fails if an input node is already added, as this node supports one input only
    //! \warning This function must be called once since an output requires one input
    virtual void AddInput(NodeIn inputNode);


    //! Update the node internal state by pulling external parameters.
    //! \note Does only call Update() for the input node.
    //!       That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data of the input node are dirty or if any input node is.
    virtual bool Update();

    //! Return the node up-to-date data.
    //! \note Defines the standard behavior of all output nodes.
    //!       Calls GetUpdatedData() on the input.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the input node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging to the input node
    //!         or to one of its input nodes. null reference if the input node is missing
    //!         (throws an assertion error in that case)
    virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------

protected:

    //! Destructor
    virtual ~OutputNode();


    //! Allocate the data associated with the node
    //! \warning This function is overridden here to throw an assertion error.
    //!          It is not supposed to be used on output nodes
    //! \return Pointer to the data being allocated
    virtual NodeData * AllocateData() const;

    //! Generate the content of the data associated with the node
    //! \warning This function is overridden here to throw an assertion error.
    //!          It is not supposed to be used on output nodes
    virtual void GenerateData();

    //! Called when an input node is going to be removed, to update the internal state
    //! \note The override of this function is optional, the default behavior does nothing
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(OutputNode)
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_OUTPUTNODE_H
