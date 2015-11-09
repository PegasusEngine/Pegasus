/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeManager.cpp
//! \author	Karolyn Boulanger
//! \date	03rd November 2013
//! \brief	Global node manager, including the factory features

#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Utils/String.h"

#include <string.h>

namespace Pegasus {
namespace Graph {


NodeManager::NodeManager(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   mNodeAllocator(nodeAllocator),
    mNodeDataAllocator(nodeDataAllocator),
    mNumRegisteredNodes(0)
{
    PG_ASSERTSTR(nodeAllocator != nullptr, "Invalid node allocator given to the NodeManager");
    PG_ASSERTSTR(nodeDataAllocator != nullptr, "Invalid node data allocator given to the NodeManager");
}

//----------------------------------------------------------------------------------------

NodeManager::~NodeManager()
{
}

//----------------------------------------------------------------------------------------

void NodeManager::RegisterNode(const char * className, Node::CreateNodeFunc createNodeFunc)
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to register a node class but the name is undefined");
        return;
    }

    if (Pegasus::Utils::Strlen(className) < 4)
    {
        PG_FAILSTR("Trying to register a node class but the name (%s) is too short", className);
        return;
    }

    if (createNodeFunc == nullptr)
    {
        PG_FAILSTR("Trying to register a node class but the factory function is undefined");
        return;
    }

    if (mNumRegisteredNodes >= MAX_NUM_REGISTERED_NODES)
    {
        PG_FAILSTR("Unable to register a node, the maximum number of nodes (%d) has been reached", MAX_NUM_REGISTERED_NODES);
        return;
    }

    // After the parameters have been validated, register the class
    NodeEntry & entry = mRegisteredNodes[mNumRegisteredNodes];
#if PEGASUS_COMPILER_MSVC
    strncpy_s(entry.className, MAX_CLASS_NAME_LENGTH + 1, className, MAX_CLASS_NAME_LENGTH + 1);
#else
    entry.className[MAX_CLASS_NAME_LENGTH] = '\0';
    strncpy(entry.className, className, MAX_CLASS_NAME_LENGTH);
#endif  // PEGASUS_COMPILER_MSVC
    entry.createNodeFunc = createNodeFunc;
    ++mNumRegisteredNodes;
}

//----------------------------------------------------------------------------------------

NodeReturn NodeManager::CreateNode(const char * className)
{
    const unsigned int registeredNodeIndex = GetRegisteredNodeIndex(className);
    if (registeredNodeIndex < mNumRegisteredNodes)
    {
        NodeEntry & entry = mRegisteredNodes[registeredNodeIndex];
        PG_ASSERT(entry.createNodeFunc != nullptr);
        return entry.createNodeFunc(mNodeAllocator, mNodeDataAllocator);
    }
    else
    {
        if (className == nullptr)
        {
            PG_FAILSTR("Unable to create a node because the provided class name is invalid");
        }
        else
        {
            PG_FAILSTR("Unable to create the node of class %s because it has not been registered", className);
        }

        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

unsigned int NodeManager::GetRegisteredNodeIndex(const char * className) const
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to find a node class by name but the name is undefined");
        return mNumRegisteredNodes;
    }

    unsigned int index;
    for (index = 0; index < mNumRegisteredNodes; ++index)
    {
        if (Pegasus::Utils::Strcmp(mRegisteredNodes[index].className, className) == 0)
        {
            // Node found
            return index;
        }
    }

    // Node not found
    PG_ASSERT(index == mNumRegisteredNodes);
    return mNumRegisteredNodes;
}


}   // namespace Graph
}   // namespace Pegasus
