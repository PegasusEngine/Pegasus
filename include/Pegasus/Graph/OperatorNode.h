/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	OperatorNode.h
//! \author	Kevin Boulanger
//! \date	20th October 2013
//! \brief	Base operator node class, for all operators with at least one input

#ifndef PEGASUS_GRAPH_OPERATORNODE_H
#define PEGASUS_GRAPH_OPERATORNODE_H

#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


//! Base operator node class, for all operators with at least one input
class OperatorNode : public Node
{
public:

    //! Default constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    OperatorNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Append a node to the list of input nodes
    //! \param inputNode Node to add to the list of input nodes (equivalent to NodeIn)
    //! \warning Fails when the current number of input nodes is already equal to \a GetMaxNumInputNodes()
    //! \warning This function must be called at least once since an operator requires at least one input
    virtual void AddInput(NodeIn inputNode);

    //! Specifies the minimum number of input nodes accepted by the current node
    //! \warning To be reimplemented in the derived class to define the behavior
    //! \return Minimum number of nodes accepted by the current node
    //!        (0 <= number < MAX_NUM_INPUTS)
    virtual unsigned int GetMinNumInputNodes() const = 0;

    //! Specifies the maximum number of input nodes accepted by the current node
    //! \warning To be reimplemented in the derived class to define the behavior
    //! \return Maximum number of nodes accepted by the current node
    //!         (GetMinNumInputNodes() <= number < MAX_NUM_INPUTS)
    virtual unsigned int GetMaxNumInputNodes() const = 0;


    //! Update the node internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \note To be redefined in derived classes if required.
    //!       In that case, this Update() should be called first, then the dirty flag
    //!       can be ORed with the content of the override before returning it.
    //! \note This class implements the default behavior of an operator:
    //!       if any input is dirty, invalidate the node data and return true,
    //!       otherwise return the dirty state of the data
    //! \return True if the node data are dirty or if any input node is.
    virtual bool Update();

    //! Return the node up-to-date data.
    //! \note Defines the standard behavior of all operator nodes.
    //!       Calls GetUpdatedData() on all inputs, and if any of them was dirty,
    //!       then calls GenerateData() to update the node data.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note When asking for the data of a node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~OperatorNode();


    //! Allocate the data associated with the node
    //! \warning To be redefined by each class defining a new class for its data
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual NodeData * AllocateData() const = 0;

    //! Generate the content of the data associated with the node
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //! Called when an input node is going to be removed, to update the internal state
    //! \note The override of this function is optional, the default behavior does nothing
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(OperatorNode)
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_OPERATORNODE_H
