/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Mesh.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh output node

#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Mesh/IMeshFactory.h"

namespace Pegasus {
namespace Mesh {


Mesh::Mesh(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
    //! \todo Add proxy and tracker (see Texture.cpp)
}

//----------------------------------------------------------------------------------------

void Mesh::SetConfiguration(const MeshConfiguration & configuration)
{
    mConfiguration = configuration;
}

//----------------------------------------------------------------------------------------

void Mesh::SetGeneratorInput(MeshGeneratorIn meshGenerator)
{
    RemoveAllInputs();
    AddInput(meshGenerator);
    SetConfiguration(meshGenerator->GetConfiguration());
}

//----------------------------------------------------------------------------------------

void Mesh::SetOperatorInput(MeshOperatorIn meshOperator)
{
    RemoveAllInputs();
    AddInput(meshOperator);
}

//----------------------------------------------------------------------------------------

MeshDataReturn Mesh::GetUpdatedMeshData()
{
    PG_ASSERT(mFactory);
    bool updated = false;
    MeshDataRef meshData = Graph::OutputNode::GetUpdatedData(updated);
    if (meshData->IsGPUDataDirty())
    {
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('MESH', "Generating the GPU data of mesh \"%s\"", GetName());
#else
        PG_LOG('MESH', "Generating the GPU data of a mesh");
#endif

        mFactory->GenerateMeshGPUData(&(*meshData));
    }
    return meshData;
}

//----------------------------------------------------------------------------------------

void Mesh::ReleaseDataAndPropagate()
{
    //! \todo See note in ReleaseGPUData()
    ReleaseGPUData();

    Graph::Node::ReleaseDataAndPropagate();
}

//----------------------------------------------------------------------------------------

void Mesh::ReleaseGPUData()
{
    //! \todo Investigate and optimize this function.
    //!       This function assumes node GPU data exists only once for the graph used by the mesh.
    //!       This function can destroy GPU data of another graph sharing the same node.
    //!       GetUpdatedData() is called twice, and the first call might generate the data
    //!       of the graph that could have been empty, to release the content right after.

    bool dummyVariable = false;
    if (GetNumInputs() == 1 && GetInput(0)->GetUpdatedData(dummyVariable) != nullptr && mFactory != nullptr)
    {
        mFactory->DestroyNodeGPUData((MeshData*)&(*GetInput(0)->GetUpdatedData(dummyVariable)));
    }
}

//----------------------------------------------------------------------------------------

Mesh::~Mesh()
{
    ReleaseGPUData();
}


}   // namespace Mesh
}   // namespace Pegasus
