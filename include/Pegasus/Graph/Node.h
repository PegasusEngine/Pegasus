/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Node.h
//! \author	Kevin Boulanger
//! \date	02nd October 2013
//! \brief	Base node class for all graph-based systems (textures, meshes, etc.)

#ifndef PEGASUS_GRAPH_NODE_H
#define PEGASUS_GRAPH_NODE_H

#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/Core/Ref.h"

namespace Pegasus {
namespace Graph {


//! Base node class for all graph-based systems (textures, meshes, shaders, etc.)
class Node
{
    friend class Pegasus::Core::Ref<Node>;

public:

    //! Default constructor
    Node();


    //! Append a node to the list of input nodes
    //! \param inputNode Node to add to the list of input nodes (equivalent to NodeIn)
    //! \warning Fails when the current number of input nodes is already MAX_NUM_INPUTS
    virtual void AddInput(const Pegasus::Core::Ref<Node> & inputNode);

    //! Get the number of input nodes connected to the current node
    //! \return Number of input nodes (from 0 to MAX_NUM_INPUTS - 1)
    inline unsigned int GetNumInputs() const { return mNumInputs; }

            
    //! Update the node internal state by pulling external parameters.
    //! \warning To be redefined in derived classes
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data are dirty or if any input node is.
    virtual bool Update() = 0;

    //! Return the node up-to-date data.
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
    virtual void ReleaseDataAndPropagate();


#if PEGASUS_DEV
    //! Set the name of the node
    //! \param name New name of the node (MAX_NAME_LENGTH characters max),
    //!             can be an empty string to delete the name
    void SetName(const char * name);

    //! Get the name of the node
    //! \return Name of the node if defined, empty string otherwise (not nullptr)
    inline const char * GetName() const { return mName; }

    //! Test if the name of the node is defined
    //! \return True if the name is not an empty string
    inline bool IsNameDefined() const { return (mName[0] != '\0'); }
#endif  // PEGASUS_DEV

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~Node();


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
    inline bool AreDataAllocated() const { return mData != nullptr; }

    //! Test if the node data are dirty
    //! \return True if the node data are dirty or unallocated
    inline bool AreDataDirty() const { return (mData != nullptr) ? mData->AreDirty() : true; }

    //! Set the dirty flag of the node data if allocated, keep it set when not allocated
    void InvalidateData();

    //! Deallocate the data, set the dirty flag of the node data at the same time
    inline void ReleaseData() { mData = nullptr; }


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

    //! Remove an input node by pointer
    //! \param inputNode Input node to remove from the current node (equivalent to NodeIn)
    //! \note Removes every instance of the given input node
    //! \note Throws an assertion if the node is not attached, then ignores the request
    //! \note \a OnRemoveInput() is called on every removed input node
    void RemoveInput(const Pegasus::Core::Ref<Node> & inputNode);

    //! Called when an input node is going to be removed, to update the internal state
    //! \note The override of this function is optional, the default behavior does nothing
    //! \param index Index of the node before it is removed
    virtual void OnRemoveInput(unsigned int index);


#if PEGASUS_DEV
    //! Return the DOT representation of the node
    //virtual const char * GetDOTDescription() const;

    //! Maximum length of the node name
    enum { MAX_NAME_LENGTH = 32 };

    //! Maximum length of the node DOT description
    enum { MAX_DOT_DESCRIPTION_LENGTH = 96 };
#endif  // PEGASUS_DEV

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(Node)


    //! Increment the reference counter, used by Ref<Node>
    inline void AddRef() { mRefCount++; }

    //! Get the current reference counter
    //! \return Number of Ref<Node> objects pointing to the current object (>= 0)
    inline int GetRefCount() const { return mRefCount; }

    //! Decrease the reference counter, and delete the current object
    //! if the counter reaches 0
    void Release();

    //! Release all input nodes
    //! \note \a OnRemoveInput() is called on every input node between removal
    void ReleaseAllInputs();


    //! Reference counter
    //! \todo Use atomic integer
    /****/int mRefCount;

    //! Maximum number of input nodes
    enum { MAX_NUM_INPUTS = 8 };

    //! Pointers to the input nodes (only the first mNumInputs nodes are valid)
    Pegasus::Core::Ref<Node> mInputs[MAX_NUM_INPUTS];

    //! Number of used input nodes (0 to MAX_NUM_INPUTS)
    unsigned int mNumInputs;

    //! Data node, used to store optional intermediate node data
    NodeDataRef mData;

#if PEGASUS_DEV
    //! Name of the node (includes the terminating character)
    char mName[MAX_NAME_LENGTH + 1];

    //! Description of the node in DOT language
    char mDOTDescription[MAX_DOT_DESCRIPTION_LENGTH];
#endif  // PEGASUS_DEV

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
