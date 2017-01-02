/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	OutputNode.h
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Base output node class, for the root of the graphs

#ifndef PEGASUS_GRAPH_OUTPUTNODE_H
#define PEGASUS_GRAPH_OUTPUTNODE_H

#include "Pegasus/Graph/Node.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"

namespace Pegasus {
namespace Graph {

//forward declaration
class NodeManager;


//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class OutputNode : public Node, public AssetLib::RuntimeAssetObject
{
    BEGIN_DECLARE_PROPERTIES(OutputNode, Node)
    END_DECLARE_PROPERTIES()

public:

    //! Default constructor
    //! \param nodeManager used to create new nodes within this node.
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    OutputNode(NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Update the node internal state by pulling external parameters.
    //! \note Does only call Update() for the input node.
    //!       That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data of the input node is dirty or if any input node is.
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
    //! \warning The \a updated output parameter must be set to false by the first caller
    virtual NodeDataReturn GetUpdatedData(bool & updated);

    //! Return the node up-to-date data.
    //! \note Defines the standard behavior of all output nodes.
    //!       Calls GetUpdatedData() on the input.
    //!       It should be overridden only for special cases.
    //! \return Reference to the node data, belonging to the input node
    //!         or to one of its input nodes. null reference if the input node is missing
    //!         (throws an assertion error in that case)
    virtual NodeDataReturn GetUpdatedData();

    //------------------------------------------------------------------------------------

    //! callback to implement reading / parsing an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to read from
    virtual bool OnReadAsset(AssetLib::AssetLib* lib, const AssetLib::Asset* asset) override;

    //! callback that writes to an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to write to
    virtual void OnWriteAsset(AssetLib::AssetLib* lib, AssetLib::Asset* asset) override;

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


    //! Append a node to the list of input nodes
    //! \param inputNode Node to add to the list of input nodes (equivalent to NodeIn),
    //!        must be non-null
    //! \warning Fails if an input node is already added, as this node supports one input only
    //! \warning This function must be called once since an output requires one input
    virtual void AddInput(NodeIn inputNode);

    //! Replace an input node by another one
    //! \param index Index of the input node to replace (== 0 as this node supports one input only)
    //! \param inputNode Replacement node (equivalent to NodeIn), must be non-null
    virtual void ReplaceInput(unsigned int index, const Pegasus::Core::Ref<Node> & inputNode);

    //! Called when an input node is going to be removed, to update the internal state
    //! \note The override of this function is optional, the default behavior does nothing
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);

#if PEGASUS_ENABLE_PROXIES
    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return GetName(); }
#endif

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(OutputNode)

    //! Node manager reference
    NodeManager* mNodeManager;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_OUTPUTNODE_H
