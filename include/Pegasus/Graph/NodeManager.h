/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeManager.h
//! \author	Karolyn Boulanger
//! \date	03rd November 2013
//! \brief	Global node manager, including the factory features

#ifndef PEGASUS_GRAPH_NODEMANAGER_H
#define PEGASUS_GRAPH_NODEMANAGER_H

#include "Pegasus/Graph/Node.h"

namespace Pegasus {
namespace Graph {


//! Global node manager, including the factory features
class NodeManager
{
public:

    //! Default constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    NodeManager(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Destructor
    virtual ~NodeManager();


    //! Register a node class, to be called before any node of this type is created
    //! \param className String of the node class (maximum length MAX_CLASS_NAME_LENGTH)
    //! \param createNodeFunc Pointer to the node member function that instantiates the node
    //! \warning If the number of registered node classes reaches MAX_NUM_REGISTERED_NODES,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of MAX_NUM_REGISTERED_NODES
    void RegisterNode(const char * className, Node::CreateNodeFunc createNodeFunc);

    //! Create a node by class name
    //! \param className Name of the node class to instantiate
    //! \return Reference to the created node, null reference if an error occurred
    NodeReturn CreateNode(const char * className);

    //------------------------------------------------------------------------------------
    
private:

    // The node manager is unique in each application
    PG_DISABLE_COPY(NodeManager)

    //! Find a registered node by name
    //! \param className Name of the class of the node to find
    //! \return Index of the node in the \a mRegisteredNodes array if found,
    //!         mNumRegisteredNodes if not found
    unsigned int GetRegisteredNodeIndex(const char * className) const;


    //! Allocator used for node internal data (except the attached NodeData)
    Alloc::IAllocator* mNodeAllocator;

    //! Allocator used for NodeData
    Alloc::IAllocator* mNodeDataAllocator;


    //! Maximum length of a class name string
    enum { MAX_CLASS_NAME_LENGTH = 63 };

    //! Maximum number of nodes that can be registered
    enum { MAX_NUM_REGISTERED_NODES = 256 };

    //! Structure describing one registered node class
    struct NodeEntry
    {
        char className[MAX_CLASS_NAME_LENGTH + 1];      //!< Name of the class
        Node::CreateNodeFunc createNodeFunc;            //!< Factory function of the node

        NodeEntry() { className[0] = '\0'; createNodeFunc = nullptr; }  //!< Default constructor
    };

    //! List of registered nodes, only the first mNumRegisteredNodes ones are valid
    NodeEntry mRegisteredNodes[MAX_NUM_REGISTERED_NODES];

    //! Number of currently registered nodes (<= MAX_NUM_REGISTERED_NODES)
    unsigned int mNumRegisteredNodes;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_NODEMANAGER_H
