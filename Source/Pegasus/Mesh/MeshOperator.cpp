/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshOperator.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Base mesh operator node class

#include "Pegasus/Mesh/MeshOperator.h"

namespace Pegasus {
namespace Mesh {


MeshOperator::MeshOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
}

//----------------------------------------------------------------------------------------

MeshOperator::MeshOperator(const MeshConfiguration & configuration,
                                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

void MeshOperator::SetConfiguration(const MeshConfiguration & configuration)
{
    if (GetRefCount() == 1)
    {
        mConfiguration = configuration;
    }
    else
    {
        PG_FAILSTR("Cannot set the configuration of a mesh operator because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------

MeshOperator::~MeshOperator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * MeshOperator::AllocateData() const
{
    return PG_NEW(GetNodeAllocator(), -1, "MeshOperator::MeshData", Pegasus::Alloc::PG_MEM_TEMP)
                    MeshData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Mesh
}   // namespace Pegasus
