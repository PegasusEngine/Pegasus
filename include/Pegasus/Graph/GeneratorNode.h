/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeneratorNode.h
//! \author	Kevin Boulanger
//! \date	19th October 2013
//! \brief	Base generator node class, for all data generators with no input node

#ifndef PEGASUS_GRAPH_GENERATORNODE_H
#define PEGASUS_GRAPH_GENERATORNODE_H

#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


//! Base generator node class, for all data generators with no input node
class GeneratorNode : public Node
{
public:

    //! Default constructor
    GeneratorNode();

    //! Override the function to add an input to prevent it to be called.
    //! \warning Fails since generators cannot have inputs and does not add the node as input
    virtual void AddInput(NodeIn inputNode);


    //! Update the node internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data are dirty or if any input node is.
    //virtual bool Update() = 0;

    //! Return the node up-to-date data.
    //! \note Defines the standard behavior of all operator nodes.
    //!       Calls GenerateData() if the node data is dirty.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note When asking for the data of a node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    virtual NodeDataReturn GetUpdatedData(bool & updated);

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    //virtual void ReleaseDataAndPropagate();

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~GeneratorNode();


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

#if PEGASUS_DEV
    //! Called when an input node is going to be removed, to update the internal state
    //! \warning This function is overridden here to throw an assertion error.
    //!          It is not supposed to be used on generator nodes
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);
#endif

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(GeneratorNode)
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_GENERATORNODE_H
