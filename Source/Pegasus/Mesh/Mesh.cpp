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
    if (updated)
    {
        mFactory->GenerateMeshGPUData((MeshData*)&(*meshData));
    }
    return meshData;
}

//----------------------------------------------------------------------------------------

void Mesh::ReleaseDataAndPropagate()
{
    //PG_TODO: easier way to delete the internals of an output node?
    bool dummyVariable = false;
    if (GetNumInputs() == 1 && GetInput(0)->GetUpdatedData(dummyVariable) != nullptr && mFactory != nullptr)
    {
        mFactory->DestroyNodeGPUData((MeshData*)&(*GetInput(0)->GetUpdatedData(dummyVariable)));
    }
}

//----------------------------------------------------------------------------------------

Mesh::~Mesh()
{
    ReleaseDataAndPropagate();
}


}   // namespace Mesh
}   // namespace Pegasus
