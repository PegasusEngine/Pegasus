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

namespace Pegasus {
namespace Mesh {


Mesh::Mesh(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
}

//----------------------------------------------------------------------------------------

Mesh::Mesh(const MeshConfiguration & configuration,
                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OutputNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

void Mesh::SetConfiguration(const MeshConfiguration & configuration)
{
    if (GetNumInputs() == 0)
    {
        mConfiguration = configuration;
    }
    else
    {
        PG_FAILSTR("Cannot set the configuration of a mesh because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------

void Mesh::SetGeneratorInput(MeshGeneratorIn meshGenerator)
{
    // Check that the configuration is compatible
  //  if (meshGenerator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(meshGenerator);
    }
    /*else
    {
        PG_FAILSTR("Unable to set the generator input of a Mesh node since their configurations are incompatible");
    }*/
}

//----------------------------------------------------------------------------------------

void Mesh::SetOperatorInput(MeshOperatorIn meshOperator)
{
    // Check that the configuration is compatible
  //  if (meshOperator->GetConfiguration().IsCompatible(this->GetConfiguration()))
    {
        RemoveAllInputs();
        AddInput(meshOperator);
    }
   /* else
    {
        PG_FAILSTR("Unable to set the operator input of a Mesh node since their configurations are incompatible");
    }*/
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
