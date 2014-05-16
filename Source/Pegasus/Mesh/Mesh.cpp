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
    bool updated = false;
    return Graph::OutputNode::GetUpdatedData(updated);
}

//----------------------------------------------------------------------------------------

Mesh::~Mesh()
{
}


}   // namespace Mesh
}   // namespace Pegasus
