/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Node.cpp
//! \author	Kevin Boulanger
//! \date	02nd October 2013
//! \brief	Base node class for all graph-based systems (textures, meshes, etc.)

#include "Pegasus/Graph/Node.h"

#include <string.h>

namespace Pegasus {
namespace Graph {


Node::Node(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   mNodeAllocator(nodeAllocator),
    mNodeDataAllocator(nodeDataAllocator),
    mRefCount(0),
    mNumInputs(0),
    mPropertyGrid()
{
    PG_ASSERTSTR(nodeAllocator != nullptr, "Invalid node allocator given to a Node");
    PG_ASSERTSTR(nodeDataAllocator != nullptr, "Invalid node data allocator given to a Node");

#if PEGASUS_ENABLE_PROXIES
    mName[0] = '\0';

    // Create the DOT description of the node (name between quotes)
    //! \todo Handle DOT
    mDOTDescription[0] = '\0';
    //strncpy(mDOTDescription, "\"", MAX_DOT_DESCRIPTION_LENGTH);
    //strlcat(mDOTDescription, mName, MAX_DOT_DESCRIPTION_LENGTH);
    //strlcat(mDOTDescription, "\"", MAX_DOT_DESCRIPTION_LENGTH);
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Node::~Node()
{
    //! \todo Use the actual integer part of the atomic int in the assert message
    PG_ASSERTSTR(mRefCount == 0, "Trying to destroy a Node that still has owners (mRefCount == %d)", mRefCount);

    // Release all inputs
    if (mNumInputs > 0)
    {
        RemoveAllInputs();
    }

    // Destroy the node data if present
    ReleaseData();
}

//----------------------------------------------------------------------------------------

void Node::InitProperties()
{
}

//----------------------------------------------------------------------------------------

NodeDataReturn Node::GetUpdatedData(bool & updated)
{
    // If the data is allocated and not dirty, it can be returned directly
    if (IsDataAllocated() && !mData->IsDirty())
    {
        return mData;
    }

    // If the data is not allocated, it has to be recomputed, but that has to be done by derived nodes
#if PEGASUS_ENABLE_PROXIES
    if (IsNameDefined())
    {
        PG_FAILSTR("Trying to recompute the data of the node \"%s\" but it should be done in a derived class.", GetName());
    }
    else
#endif  // PEGASUS_ENABLE_PROXIES
    {
        PG_FAILSTR("Trying to recompute the data of a node but it should use a derived class.");
    }

    // Return the data even if nullptr, but this code should never be reached
    updated = false;
    return mData;
}

//----------------------------------------------------------------------------------------

void Node::ReleaseDataAndPropagate()
{
    // Deallocate the data if defined
    ReleaseData();

    // Default behavior, ask each input node to release its data
    for (unsigned int i = 0; i < mNumInputs; ++i)
    {
        GetInput(i)->ReleaseDataAndPropagate();
    }
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void Node::SetName(const char * name)
{
    if (name == nullptr)
    {
        mName[0] = '\0';
    }
    else
    {
#if PEGASUS_COMPILER_MSVC
        strncpy_s(mName, MAX_NAME_LENGTH, name, MAX_NAME_LENGTH);
#else
        mName[MAX_NAME_LENGTH - 1] = '\0';
        strncpy(mName, name, MAX_NAME_LENGTH - 1);
#endif  // PEGASUS_COMPILER_MSVC
    }
}
#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

void Node::CreateData()
{
    if (!IsDataAllocated())
    {
        NodeData * newData = AllocateData();
        if (newData != nullptr)
        {
            mData = newData;
        }
        else
        {
            PG_FAILSTR("Error when allocating the data of a node");
        }
    }
    else
    {
        PG_FAILSTR("Trying to allocate the data of a node, but the previous data is still allocated. Keeping and invalidating the previous data");

        // Invalidate the data that should not be there in the first place
        mData->Invalidate();
    }
}

//----------------------------------------------------------------------------------------

void Node::InvalidateData()
{
    if (mData != nullptr)
    {
        mData->Invalidate();
    }
}

//----------------------------------------------------------------------------------------

void Node::AddInput(const Pegasus::Core::Ref<Node> & inputNode)
{
    if (inputNode == nullptr)
    {
        PG_FAILSTR("Invalid node added as an input to the current node");
        return;
    }

    if (mNumInputs >= MAX_NUM_INPUTS)
    {
        PG_FAILSTR("Too many nodes added to the current node");
        return;
    }

    mInputs[mNumInputs] = inputNode;
    ++mNumInputs;

    // Since an input node has been added, that means the node data is dirty
    if (mData != nullptr)
    {
        mData->Invalidate();
    }
}

//----------------------------------------------------------------------------------------

void Node::ReplaceInput(unsigned int index, const Pegasus::Core::Ref<Node> & inputNode)
{
    if (index >= mNumInputs)
    {
        PG_FAILSTR("Invalid input index (%d) when trying to replace an input node, it must be < %d", index, GetNumInputs());
        return;
    }

    if (inputNode == nullptr)
    {
        PG_FAILSTR("Invalid node added as an input to the current node");
        return;
    }

    if (mInputs[index] == nullptr)
    {
        PG_FAILSTR("Corrupted node, one of the inputs is undefined");
    }

    if (inputNode != mInputs[index])
    {
        // Replace the node (releases the previous node)
        mInputs[index] = inputNode;

        // Since an input node has been changed, that means the node data is dirty
        if (mData != nullptr)
        {
            mData->Invalidate();
        }
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Core::Ref<Node> Node::GetInput(unsigned int index) const
{
    if (index < GetNumInputs())
    {
        PG_ASSERTSTR(mInputs[index] != nullptr, "Trying to get the input node with index %d, but the node pointer is invalid", index);
        return mInputs[index];
    }
    else
    {
        PG_FAILSTR("Trying to get the input node with index %d, but there are only %d inputs attached", index, mNumInputs);
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

bool Node::IsInput(const Pegasus::Core::Ref<Node> & inputNode) const
{
    PG_ASSERTSTR(inputNode != nullptr, "Trying to test if a nullptr node is an input of another one");

    if (inputNode != nullptr)
    {
        for (unsigned int i = 0; i < mNumInputs; ++i)
        {
            if (mInputs[i] == inputNode)
            {
                return true;
            }
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

void Node::RemoveInput(const Pegasus::Core::Ref<Node> & inputNode)
{
    PG_ASSERTSTR(inputNode != nullptr, "Trying to remove a nullptr node from the list of inputs of another node");
    bool nodeFound = false;

    if (inputNode != nullptr)
    {
        for (int i = (int)mNumInputs - 1; i >= 0; --i)
        {
            PG_ASSERTSTR(mInputs[i] != nullptr, "Invalid input of a node while trying to remove another node from the list of inputs of the current one");

            if ((mInputs[i] != nullptr) && (mInputs[i] == inputNode))
            {
                // Input node found
                nodeFound = true;

                // Call the optional function that handles the internal state
                // of the current node when an input is removed
                OnRemoveInput((unsigned int)i);

                // Remove the input node
                mInputs[i] = nullptr;
                --mNumInputs;

                // Move the input nodes after the removed node one step down
                for (int j = i; j < (int)mNumInputs; ++j)
                {
                    mInputs[j] = mInputs[j + 1];
                }

                // Remove the duplicated reference at the end of the array
                mInputs[mNumInputs] = nullptr;
            }
        }
    }

    // If an input node has been removed, that means the node data is dirty
    if (nodeFound && (mData != nullptr))
    {
        mData->Invalidate();
    }

    PG_ASSERTSTR(nodeFound, "Trying to remove an input node from the current node, but the input node has not been found");
}

//----------------------------------------------------------------------------------------

void Node::RemoveAllInputs()
{
    for (int i = (int)mNumInputs - 1; i >= 0; --i)
    {
        if (mInputs[i] != nullptr)
        {
            // Call the optional function that handles the internal state
            // of the current node when an input is removed
            OnRemoveInput((unsigned int)i);

            // Remove the input node
            mInputs[i] = nullptr;
        }
        else
        {
            PG_FAILSTR("Invalid input node while removing all input nodes");
        }

        --mNumInputs;
    }
}

//----------------------------------------------------------------------------------------

void Node::OnRemoveInput(unsigned int index)
{
    // Empty default behavior
}

//----------------------------------------------------------------------------------------

void Node::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        //! \todo The destructor is called explicitly here because PG_DELETE does not do it.
        //!       This should be replaced by implicit destructors
        this->~Node();
        PG_DELETE(mNodeAllocator, this);
    }
}


}   // namespace Graph
}   // namespace Pegasus
