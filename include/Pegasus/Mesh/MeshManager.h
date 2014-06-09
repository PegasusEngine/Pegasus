/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshManager.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Global mesh node manager, including the factory features

#ifndef PEGASUS_MESH_MESHMANAGER_H
#define PEGASUS_MESH_MESHMANAGER_H


#include "Pegasus/Graph/Node.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Mesh/MeshOperator.h"

namespace Pegasus {
    namespace Graph {
        class NodeManager;
    }
}

namespace Pegasus {
namespace Mesh {

class IMeshFactory;

//! Global mesh node manager, including the factory features
class MeshManager
{
public:

    //! Constructor
    //! \note Registers all mesh nodes of the Mesh project
    //! \param nodeManager Pointer to the global node manager (!- nullptr)
    MeshManager(Graph::NodeManager * nodeManager, IMeshFactory * factory);

    //! Destructor
    virtual ~MeshManager();


    //! Register a mesh node class, to be called before any node of this type is created
    //! \param className String of the node class (maximum length NodeManager::MAX_CLASS_NAME_LENGTH)
    //! \param createNodeFunc Pointer to the mesh node member function that instantiates the node
    //! \warning If the number of registered node classes reaches NodeManager::MAX_NUM_REGISTERED_NODES,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of NodeManager::MAX_NUM_REGISTERED_NODES
    void RegisterMeshNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc);

    //! Create a mesh node
    //! \param configuration Configuration of the mesh
    //! \return Reference to the created node, null reference if an error occurred
    MeshReturn CreateMeshNode();

    //! Create a mesh generator node by class name
    //! \param className Name of the mesh generator node class to instantiate
    //! \return Reference to the created node, null reference if an error occurred
    MeshGeneratorReturn CreateMeshGeneratorNode(const char * className);

    //! Create an mesh operator node by class name
    //! \param className Name of the mesh operator node class to instantiate
    //! \param configuration Configuration of the mesh
    //! \return Reference to the created node, null reference if an error occurred
    MeshOperatorReturn CreateMeshOperatorNode(const char * className,
                                                    const MeshConfiguration & configuration);

#if PEGASUS_USE_GRAPH_EVENTS
    //! Registers an event listener so we can listen to mesh specific event whilst constructing nodes.
    //! \param the event listener to use
    void RegisterEventListener(IMeshEventListener * eventListener) { mEventListener = eventListener; }
#endif
    
private:

    // The mesh node manager is unique in each application
    PG_DISABLE_COPY(MeshManager)

        
    //! Register all the mesh nodes of the Mesh project
    void RegisterAllMeshNodes();

    //! Pointer to the node manager (!= nullptr)
    Graph::NodeManager * mNodeManager;

    //! Pointer to the GPU factory. Generates GPU data from cpu mesh data
    IMeshFactory * mFactory;

#if PEGASUS_USE_GRAPH_EVENTS
    IMeshEventListener * mEventListener;
#endif
};


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHMANAGER_H
