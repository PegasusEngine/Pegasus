/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Node.cpp
//! \author	Karolyn Boulanger
//! \date	02nd October 2013
//! \brief	Base node class for all graph-based systems (textures, meshes, etc.)

#include "Pegasus/Graph/Node.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus::AssetLib;

namespace Pegasus {
namespace Graph {


BEGIN_IMPLEMENT_PROPERTIES(Node)
END_IMPLEMENT_PROPERTIES(Node)

//----------------------------------------------------------------------------------------

Node::Node(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Core::RefCounted(nodeAllocator)
,   mNodeAllocator(nodeAllocator)
,   mNodeDataAllocator(nodeDataAllocator)
,   mNumInputs(0)
{
    BEGIN_INIT_PROPERTIES(Node)
    END_INIT_PROPERTIES()

    PG_ASSERTSTR(nodeAllocator != nullptr, "Invalid node allocator given to a Node");
    PG_ASSERTSTR(nodeDataAllocator != nullptr, "Invalid node data allocator given to a Node");

#if PEGASUS_ENABLE_PROXIES
    mNodeType = NODETYPE_UNKNOWN;
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Node::~Node()
{
    // Release all inputs
    if (mNumInputs > 0)
    {
        RemoveAllInputs();
    }

    // Destroy the node data if present
    ReleaseData();
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

NodeDataReturn Node::GetUpdatedData(bool & updated)
{
    // If the data is allocated and not dirty, it can be returned directly
    if (IsDataAllocated() && !mData->IsDirty())
    {
        return mData;
    }

    // If the data is not allocated, it has to be recomputed, but that has to be done by derived nodes
    PG_FAILSTR("Trying to recompute the data of the node \"%s\" but it should be done in a derived class.", GetName());

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


void Node::WriteToObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj) const
{
    obj->AddString("class", GetClassInstanceName());
    
    Object* propertyGridObj = parentAsset->NewObject();
    obj->AddObject("props", propertyGridObj);
    PropertyGridObject::WriteToObject(parentAsset, propertyGridObj);

    Array* inputs = parentAsset->NewArray();
    inputs->CommitType(Array::AS_TYPE_OBJECT);
    obj->AddArray("inputs", inputs);

    for (unsigned int i = 0; i < GetNumInputs(); ++i)
    {
        NodeRef n = GetInput(i);
        Object* inputObj = parentAsset->NewObject();
        n->WriteToObject(parentAsset, inputObj);

        Array::Element e;
        e.o = inputObj;
        inputs->PushElement(e);
    }

}

bool Node::ReadFromObject(NodeManager* nodeManager, AssetLib::Asset* parentAsset, AssetLib::Object* obj)
{
    int strId = obj->FindString("class");
    if (strId == -1 || Utils::Strcmp(obj->GetString(strId), GetClassInstanceName()))
    {
        PG_LOG('ERR_' , "Error while parsing node object");
        return false;
    }

    int propGridObjId = obj->FindObject("props");
    int inputsArrId = obj->FindArray("inputs");

    if (propGridObjId == -1 || inputsArrId == -1)
    {
        PG_LOG('ERR_' , "Error while parsing node object. No property grid defined.");
        return false;
    }

    Object* propGridObject = obj->GetObject(propGridObjId);
    if (!PropertyGridObject::ReadFromObject(parentAsset, obj))
    { 
        PG_LOG('ERR_' , "Error while parsing node object. Invalid property grid.");
        return false;
    }

    Array* inputs = obj->GetArray(inputsArrId);
    if (inputs->GetSize() > 0 && inputs->GetType() != Array::AS_TYPE_OBJECT)
    {
        PG_LOG('ERR_' , "Error while parsing node object: invalid input array type.");
        return false;
    }

    for (int i = 0; i < inputs->GetSize(); ++i)
    {
        const Array::Element& e = inputs->GetElement(i);
        int nodeClassId = e.o->FindString("class"); 
        if (nodeClassId == -1)
        {
            PG_LOG('ERR_' , "Error while parsing node object");
            return false;
        }

        NodeRef inputNode = nodeManager->CreateNode(e.o->GetString(nodeClassId));
        if (!inputNode->ReadFromObject(nodeManager, parentAsset, e.o))
        {
            return false;
        }
    
        AddInput(inputNode);
    }
    

    return true;

}


}   // namespace Graph
}   // namespace Pegasus
