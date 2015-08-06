/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeData.h
//! \author	Karolyn Boulanger
//! \date	17th October 2013
//! \brief	Base node data class for all graph-based systems (textures, meshes, etc.)

#ifndef PEGASUS_GRAPH_NODEDATA_H
#define PEGASUS_GRAPH_NODEDATA_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/NodeGPUData.h"

namespace Pegasus {
namespace Graph {


//! Base node data class for all graph-based systems (textures, meshes, shaders, etc.)
class NodeData
{
    template<class C> friend class Pegasus::Core::Ref;
    friend class Node;

public:

    //! Default constructor
    //! \param allocator Allocator used for the node data
    //! \note Sets the dirty flag
    NodeData(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~NodeData();

    //! Set the data as dirty, meaning it will need to be recomputed to be valid
    //! \note Does not deallocate any memory, only sets a flag
    inline void Invalidate() { mDirty = true; mGPUDataDirty = true; }

    //! Set the data as non-dirty, usually after a call to \a GenerateData()
    inline void Validate() { mDirty = false; }

    //! Set the GPU data as non-dirty
    inline void ValidateGPUData() { mGPUDataDirty = false; }

    //! Test if the data is dirty
    //! \return True if the dirty flag is set
    inline bool IsDirty() const { return mDirty; }

    //! Test if the GPU data is dirty
    //! \return True if the GPU data dirty flag is set
    inline bool IsGPUDataDirty() const { return mGPUDataDirty; }

    //! Set custom node GPU data by user code
    //! \note If nodeGPUData is set to nullptr, the GPU data is invalidated
    //! \param nodeGPUData External GPU data to store in the node data
    inline void SetNodeGPUData (NodeGPUData * nodeGPUData)
        { mNodeGPUData = nodeGPUData; if (nodeGPUData == nullptr) { mGPUDataDirty = true; } }

    //! Get custom node GPU data set by user
    //! \return External GPU data stored in the node data, can be nullptr if invalid or dirty
    inline NodeGPUData * GetNodeGPUData () { return mNodeGPUData; }

    //! Get custom read only node GPU data set by user
    //! \return External GPU data stored in the node data, can be nullptr if invalid or dirty
    inline const NodeGPUData * GetNodeGPUData () const { return mNodeGPUData; }

    //------------------------------------------------------------------------------------
    
protected:

    //! Get the allocator for the node data, to be used in every derived class
    //! \return Node allocator
    inline Alloc::IAllocator * GetAllocator() const { return mAllocator; }

    //------------------------------------------------------------------------------------
    
private:

    // Node data cannot be copied, only references to them
    PG_DISABLE_COPY(NodeData)


    //! Increment the reference counter, used by Ref<Node>
    inline void AddRef() { mRefCount++; }

    //! Get the current reference counter
    //! \return Number of Ref<Node> objects pointing to the current object (>= 0)
    inline int GetRefCount() const { return mRefCount; }

    //! Decrease the reference counter, and delete the current object
    //! if the counter reaches 0
    void Release();

    //! GPU data container
    NodeGPUData * mNodeGPUData;

    //! Allocator for this object
    Alloc::IAllocator * mAllocator;

    //! Reference counter
    //! \todo Use atomic integer
    int mRefCount;

    //! True when the data is dirty, meaning it will need to be recomputed to be valid
    bool mDirty;

    //! True when the GPU data is dirty, meaning it will need to be recomputed to be valid
    bool mGPUDataDirty;
};

//----------------------------------------------------------------------------------------

//! Reference to a NodeData, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<NodeData>   NodeDataRef;

//! Const reference to a reference to a NodeData, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<NodeData>   NodeDataIn;

//! Reference to a reference to a NodeData, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<NodeData> & NodeDataInOut;

//! Reference to a NodeData, typically used as the return value of a function
typedef       Pegasus::Core::Ref<NodeData>   NodeDataReturn;


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_NODEDATA_H
