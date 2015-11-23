/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Node.h
//! \author	Karolyn Boulanger
//! \date	02nd October 2013
//! \brief	Base node class for all graph-based systems (textures, meshes, etc.)

#ifndef PEGASUS_GRAPH_NODE_H
#define PEGASUS_GRAPH_NODE_H

#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/Core/RefCounted.h"

//forward declarations
namespace Pegasus {
namespace AssetLib {
    class Asset;
    class Object;
}
}

namespace Pegasus {
namespace Graph {

class NodeManager;


//! Base node class for all graph-based systems (textures, meshes, shaders, etc.)
class Node : public Core::RefCounted, public PropertyGrid::PropertyGridObject
{
    template<class C> friend class Pegasus::Core::Ref;

    BEGIN_DECLARE_PROPERTIES_BASE(Node)
    END_DECLARE_PROPERTIES()

public:

    //! Default constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    Node(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Destructor
    virtual ~Node();


    //! Get the number of input nodes connected to the current node
    //! \return Number of input nodes (from 0 to MAX_NUM_INPUTS - 1)
    inline unsigned int GetNumInputs() const { return mNumInputs; }

    //! Test if an input node index is valid
    //! \param index Index to test
    //! \return True if the index is valid
    inline bool IsInputIndexValid(unsigned int index) const { return (index < mNumInputs); }

    //! Get an input node
    //! \param Index of the input node to get (0 <= index < mNumInputs)
    //! \return Reference to the input node, empty reference in case of error
    //!         (equivalent to NodeReturn)
    Pegasus::Core::Ref<Node> GetInput(unsigned int index) const;

    //! Test if a node is attached as an input node of the current node
    //! \param inputNode Node to test (equivalent to NodeIn)
    //! \return True if inputNode is attached to the current node as an input node
    bool IsInput(const Pegasus::Core::Ref<Node> & inputNode) const;

            
    //! Update the node internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \warning To be redefined in derived classes
    //! \return True if the node data is dirty or if any input node is.
    virtual bool Update() = 0;

    //! Return the node up-to-date data.
    //! \param updated Set to true if the node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note When asking for the data of a node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //! \warning The \a updated output parameter must be set to false by the first caller
    virtual NodeDataReturn GetUpdatedData(bool & updated);

    //! Deallocate the data of the current node and ask the input nodes to do the same.
    //! Typically used when keeping the graph in memory but not the associated data,
    //! to save memory and to be able to restore the data later
    virtual void ReleaseDataAndPropagate();


    //! Creation function type used by the node manager
    //! \param nodeManager - the node manager used for creation
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    typedef Pegasus::Core::Ref<Node> (* CreateNodeFunc)(NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Returns the class instance name of this node.
    //! \return class instance name c string
    virtual const char* GetClassInstanceName() const = 0;

#if PEGASUS_ENABLE_PROXIES

    //! Definition of the different types of nodes
    enum NodeType
    {
        NODETYPE_GENERATOR = 0,
        NODETYPE_OPERATOR,
        NODETYPE_OUTPUT,
        NUM_NODETYPES,
        NODETYPE_UNKNOWN = NUM_NODETYPES
    };

    //! Get the type of node
    //! \return Type of the node (NODETYPE_xxx constant)
    inline NodeType GetNodeType() const { return mNodeType; }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
protected:

    //! Get the allocator used for node internal data (except the attached NodeData)
    //! \return Node allocator
    inline Alloc::IAllocator* GetNodeAllocator() const { return mNodeAllocator; }

    //! Get the allocator used for NodeData
    //! \return Node data allocator
    inline Alloc::IAllocator* GetNodeDataAllocator() const { return mNodeDataAllocator; }

    //! Allocate the data associated with the node
    //! \warning To be redefined by each class defining a new class for its data
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual NodeData * AllocateData() const = 0;

    //! Generate the content of the data associated with the node
    //! \warning To be redefined by each derived class, to implement its behavior.
    //! \note Use GetData() in the derived class, as the data is guaranteed to be allocated
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;


    //! Create the data associated with the node
    //! \warning Only calls the default constructor of the node data object,
    //!          it does not generate its content.
    //! \note Calls \a AllocateData(), which is an overridable function
    //! \note If data were already allocated, throws an assertion and keep the original
    //!       node data. Otherwise the pointers to the original data would become invalid.
    //!       In that case, the dirty flag of the node data is set
    void CreateData();

    //! Return the node data, even if dirty or unallocated
    //! \return Node data, can be nullptr
    //! \warning The data can be missing. Use \a GetUpdatedData() if up-to-date data is required
    inline NodeDataReturn GetData() const { return mData; }

    //! Test if the node data are allocated (does not test for dirtiness)
    //! \return True if the node data are allocated
    inline bool IsDataAllocated() const { return mData != nullptr; }

    //! Test if the node data is dirty
    //! \return True if the node data is dirty or unallocated
    inline bool IsDataDirty() const { return (mData != nullptr) ? mData->IsDirty() : true; }

    //! Set the dirty flag of the node data if allocated, keep it set when not allocated
    void InvalidateData();

    //! Deallocate the data, set the dirty flag of the node data at the same time
    inline void ReleaseData() { mData = nullptr; }


    //! Maximum number of input nodes
    enum { MAX_NUM_INPUTS = 8 };

    //! Append a node to the list of input nodes
    //! \param inputNode Node to add to the list of input nodes (equivalent to NodeIn),
    //!        must be non-null
    //! \warning Fails when the current number of input nodes is already MAX_NUM_INPUTS
    virtual void AddInput(const Pegasus::Core::Ref<Node> & inputNode);

    //! Replace an input node by another one
    //! \param index Index of the input node to replace (0 <= index < GetNumInputs())
    //! \param inputNode Replacement node (equivalent to NodeIn), must be non-null
    virtual void ReplaceInput(unsigned int index, const Pegasus::Core::Ref<Node> & inputNode);

    //! Remove an input node by reference
    //! \param inputNode Input node to remove from the current node (equivalent to NodeIn)
    //! \note Removes every instance of the given input node
    //! \note Throws an assertion if the node is not attached, then ignores the request
    //! \note \a OnRemoveInput() is called on every removed input node
    void RemoveInput(const Pegasus::Core::Ref<Node> & inputNode);

    //! Remove all input nodes
    //! \note \a OnRemoveInput() is called on every input node between removal
    void RemoveAllInputs();

    //! Called when an input node is going to be removed, to update the internal state
    //! \note The override of this function is optional, the default behavior does nothing
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);


    //! Called when a node is dumped into an asset
    //! \param obj - the object to write the contents of this object to.
    virtual void WriteToObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj) const;

    //! Called when a node is read from an object
    //! \parama obj - the object to read the contents from.
    virtual bool ReadFromObject(NodeManager* nodeManager, AssetLib::Asset* parentAsset, AssetLib::Object* obj);

#if PEGASUS_ENABLE_PROXIES

    //! Type of the node (NODETYPE_xxx constant)
    NodeType mNodeType;

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(Node)

    //! Allocator used for node internal data (except the attached NodeData)
    Alloc::IAllocator* mNodeAllocator;

    //! Allocator used for NodeData
    Alloc::IAllocator* mNodeDataAllocator;

    //! Pointers to the input nodes (only the first mNumInputs nodes are valid)
    Pegasus::Core::Ref<Node> mInputs[MAX_NUM_INPUTS];

    //! Number of used input nodes (0 to MAX_NUM_INPUTS)
    unsigned int mNumInputs;

    //! Data node, used to store optional intermediate node data
    NodeDataRef mData;
};

//----------------------------------------------------------------------------------------

//! Reference to a Node, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<Node>   NodeRef;

//! Const reference to a reference to a Node, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<Node> & NodeIn;

//! Reference to a reference to a Node, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<Node> & NodeInOut;

//! Reference to a Node, typically used as the return value of a function
typedef       Pegasus::Core::Ref<Node>   NodeReturn;


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_NODE_H
