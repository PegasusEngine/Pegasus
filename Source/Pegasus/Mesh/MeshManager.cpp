/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshManager.cpp
//! \author	Kleber Garcia
//! \date   11th May 2014	
//! \brief	Global mesh node manager, including the factory features

#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Mesh/Generator/QuadGenerator.h"
#include "Pegasus/Mesh/IMeshFactory.h"
#include "Pegasus/Graph/NodeManager.h"

namespace Pegasus {
namespace Mesh {


//! Macro to register a mesh node, used only in the \a RegisterAllMeshNodes() function
//! \param className Class name of the mesh node to register
#define REGISTER_MESH_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode)

//----------------------------------------------------------------------------------------
    
MeshManager::MeshManager(Graph::NodeManager * nodeManager, IMeshFactory * factory)
:   mNodeManager(nodeManager), mFactory(factory)
#if PEGASUS_USE_GRAPH_EVENTS
    //initialize without an event listener
    , mEventListener(nullptr)
#endif
{
    PG_ASSERT(factory);
    if (nodeManager != nullptr)
    {
        RegisterAllMeshNodes();
    }
    else
    {
        PG_FAILSTR("Invalid node manager given to the mesh manager");
    }
}

//----------------------------------------------------------------------------------------

MeshManager::~MeshManager()
{
}

//----------------------------------------------------------------------------------------

void MeshManager::RegisterMeshNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc)
{
    if (mNodeManager != nullptr)
    {
        mNodeManager->RegisterNode(className, createNodeFunc);
    }
    else
    {
        PG_FAILSTR("Unable to register a mesh node because the mesh manager is not linked to the node manager");
    }
}

//----------------------------------------------------------------------------------------

MeshReturn MeshManager::CreateMeshNode()
{
    if (mNodeManager != nullptr)
    {
        MeshRef mesh = mNodeManager->CreateNode("Mesh");
        mesh->SetFactory(mFactory);
        return mesh;
    }
    else
    {
        PG_FAILSTR("Unable to create a mesh node because the mesh manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

MeshGeneratorReturn MeshManager::CreateMeshGeneratorNode(const char * className)
{
    if (mNodeManager != nullptr)
    {
        //! \todo Check that the class corresponds to a generator mesh

        MeshGeneratorRef meshGenerator = mNodeManager->CreateNode(className);
#if PEGASUS_USE_GRAPH_EVENTS
        //propagate event listener
        meshGenerator->SetEventListener(mEventListener);
#endif
        return meshGenerator;
    }
    else
    {
        PG_FAILSTR("Unable to create a generator mesh node because the mesh manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

MeshOperatorReturn MeshManager::CreateMeshOperatorNode(const char * className,
                                                                const MeshConfiguration & configuration)
{
    if (mNodeManager != nullptr)
    {
        //! \todo Check that the class corresponds to an operator mesh

        MeshOperatorRef meshOperator = mNodeManager->CreateNode(className);
        meshOperator->SetConfiguration(configuration);
#if PEGASUS_USE_GRAPH_EVENTS
        //propagate event listener
        meshOperator->SetEventListener(mEventListener);
#endif
        return meshOperator;
    }
    else
    {
        PG_FAILSTR("Unable to create an operator mesh node because the mesh manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void MeshManager::RegisterAllMeshNodes()
{
    PG_ASSERTSTR(mNodeManager != nullptr, "Enable to register the mesh nodes because the mesh manager is not linke to the node manager");

    // Register the output mesh node
    REGISTER_MESH_NODE(Mesh);

    // Register the generator nodes
    // IMPORTANT! Add here every mesh generator node that is created,
    //            and update the list of #includes above
    REGISTER_MESH_NODE(QuadGenerator);

}


}   // namespace Mesh
}   // namespace Pegasus
