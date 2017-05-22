/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshManager.cpp
//! \author	Kleber Garcia
//! \date   11th May 2014	
//! \brief	Global mesh node manager, including the factory features

#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Mesh/MeshManager.h"
#include "Pegasus/Mesh/Operator/CombineTransformOperator.h"
#include "Pegasus/Mesh/Operator/MultiCopyOperator.h"
#include "Pegasus/Mesh/Operator/WaveFieldOperator.h"
#include "Pegasus/Mesh/Generator/QuadGenerator.h"
#include "Pegasus/Mesh/Generator/BoxGenerator.h"
#include "Pegasus/Mesh/Generator/IcosphereGenerator.h"
#include "Pegasus/Mesh/Generator/CustomGenerator.h"
#include "Pegasus/Mesh/IMeshFactory.h"
#include "Pegasus/Graph/NodeManager.h"
#include "Pegasus/Utils/String.h"

namespace Pegasus {
namespace Mesh {

#if PEGASUS_ENABLE_PROXIES
#define REGISTER_MESH_TYPE_NAME(className, bucket)  RegisterInNameSet(className, bucket)
#else
#define REGISTER_MESH_TYPE_NAME(className, bucket) 
#endif

//! Macro to register a mesh node, used only in the \a RegisterAllMeshNodes() function
//! \param className Class name of the mesh node to register
#define REGISTER_MESH_NODE(className) mNodeManager->RegisterNode(#className, className::CreateNode)

#define REGISTER_MESH_NODE_GENERATOR(className) mNodeManager->RegisterNode(#className, className::CreateNode);REGISTER_MESH_TYPE_NAME(#className,mGeneratorTypeNameHashes)

#define REGISTER_MESH_NODE_OPERATOR(className) mNodeManager->RegisterNode(#className, className::CreateNode);REGISTER_MESH_TYPE_NAME(#className,mOperatorTypeNameHashes)

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
bool ExistsInNameSet(const char* name, const Utils::Vector<unsigned int>& nameSet)
{
    unsigned int nameHash = Utils::HashStr(name);
    for (unsigned i = 0; i < nameSet.GetSize(); ++i)
    {
        if (nameHash == nameSet[i])
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

void RegisterInNameSet(const char* name, Utils::Vector<unsigned int>& nameSet)
{
    unsigned int nameHash = Utils::HashStr(name);
    bool exists = ExistsInNameSet(name, nameSet);
    PG_ASSERTSTR(!exists, "Node %s already exists in registered node type pool.", name);
    nameSet.PushEmpty() = nameHash;
}
#endif

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
    
MeshManager::MeshManager(Graph::NodeManager * nodeManager, IMeshFactory * factory)
:   mNodeManager(nodeManager), mFactory(factory)
#if PEGASUS_ENABLE_PROXIES
    ,mProxy(this)
#endif
#if PEGASUS_USE_EVENTS
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

void MeshManager::RegisterMeshNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc, bool isOperator)
{
    if (mNodeManager != nullptr)
    {
        mNodeManager->RegisterNode(className, createNodeFunc);
#if PEGASUS_ENABLE_PROXIES
        if (isOperator)
        {
            RegisterInNameSet(className, mOperatorTypeNameHashes);
        }
        else
        {
            RegisterInNameSet(className, mGeneratorTypeNameHashes);
        }
#endif
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

#if PEGASUS_ENABLE_PROXIES
        if (!ExistsInNameSet(className, mGeneratorTypeNameHashes))
        {
            PG_LOG('ERR_', "Attempting to create an invalid mesh opertor node %s.", className);
            return nullptr;
        }
#endif
        MeshGeneratorRef meshGenerator = mNodeManager->CreateNode(className);
#if PEGASUS_USE_EVENTS
        //propagate event listener
        meshGenerator->SetEventListener(mEventListener);
#endif
        meshGenerator->SetFactory(mFactory);
        return meshGenerator;
    }
    else
    {
        PG_FAILSTR("Unable to create a generator mesh node because the mesh manager is not linked to the node manager");
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

MeshOperatorReturn MeshManager::CreateMeshOperatorNode(const char * className)
{
    if (mNodeManager != nullptr)
    {

#if PEGASUS_ENABLE_PROXIES
        if (!ExistsInNameSet(className, mOperatorTypeNameHashes))
        {
            PG_LOG('ERR_', "Attempting to create an invalid mesh opertor node %s.", className);
            return nullptr;
        }
#endif
        MeshOperatorRef meshOperator = mNodeManager->CreateNode(className);

#if PEGASUS_USE_EVENTS
        //propagate event listener
        meshOperator->SetEventListener(mEventListener);
#endif
        meshOperator->SetFactory(mFactory);
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

    // Register the operator nodes
    REGISTER_MESH_NODE_OPERATOR(CombineTransformOperator);
    REGISTER_MESH_NODE_OPERATOR(MultiCopyOperator);
    REGISTER_MESH_NODE_OPERATOR(WaveFieldOperator);

    // Register the generator nodes
    REGISTER_MESH_NODE_GENERATOR(QuadGenerator);
    REGISTER_MESH_NODE_GENERATOR(BoxGenerator);
    REGISTER_MESH_NODE_GENERATOR(IcosphereGenerator);
    REGISTER_MESH_NODE_GENERATOR(CustomGenerator);

}

//----------------------------------------------------------------------------------------

const PegasusAssetTypeDesc*const* MeshManager::GetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gDescs[] = {
         &ASSET_TYPE_MESH
       , nullptr
    };
    return gDescs;
}

//----------------------------------------------------------------------------------------

AssetLib::RuntimeAssetObjectRef MeshManager::CreateRuntimeObject(const PegasusAssetTypeDesc* desc)
{
    if (desc->mTypeGuid == ASSET_TYPE_MESH.mTypeGuid)
    {
        MeshReturn mesh = CreateMeshNode();
        return mesh;
    }
    return nullptr;
}


}   // namespace Mesh
}   // namespace Pegasus
