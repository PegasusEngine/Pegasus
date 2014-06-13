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
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    GeneratorNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Update the node internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! and returns the dirty flag to the parent caller.
    //! That will trigger a data refresh when calling GetUpdatedData().
    //! \note To be redefined in derived classes if required.
    //!       In that case, this Update() should be called first, then the dirty flag
    //!       can be ORed with the content of the override before returning it.
    //! \note This class implements the default behavior of a generator,
    //!       which returns only the dirty state of the data
    //! \warning If this base class function is called in a derived class,
    //!       it must be called at the end, after updating the internal state
    //!       and invalidating the node data
    //! \return True if the node data is dirty
    virtual bool Update();

    //! Return the node up-to-date data.
    //! \note Defines the standard behavior of all texture generator nodes.
    //!       Calls GenerateData() if the node data is dirty.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, cannot be a null reference.
    //! \note When asking for the data of a generator node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //! \warning The \a updated output parameter must be set to false by the first caller
    virtual NodeDataReturn GetUpdatedData(bool & updated);

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


    //! Override the function to add an input to prevent it to be called.
    //! \warning Throws an assertion error since generators cannot have inputs
    //!          and does not add the node as input
    virtual void AddInput(NodeIn inputNode);

    //! Override the function to replace an input node by another one to prevent it to be called
    //! \warning Throws an assertion error since generators cannot have inputs
    virtual void ReplaceInput(unsigned int index, const Pegasus::Core::Ref<Node> & inputNode);

#if PEGASUS_ENABLE_ASSERT
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
