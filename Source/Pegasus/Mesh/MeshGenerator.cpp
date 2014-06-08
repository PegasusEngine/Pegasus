/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshGenerator.cpp
//! \author	Kleber Garcia
//! \date	5th May 2014
//! \brief	Base mesh generator node class

#include "Pegasus/Mesh/MeshGenerator.h"

namespace Pegasus {
namespace Mesh {


MeshGenerator::MeshGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration()
{
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

MeshGenerator::MeshGenerator(const MeshConfiguration & configuration,
                                   Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
    //initialize event user data
    GRAPH_EVENT_INIT_DISPATCHER
}

//----------------------------------------------------------------------------------------

void MeshGenerator::SetConfiguration(const MeshConfiguration & configuration)
{
    if (GetRefCount() == 1)
    {
        mConfiguration = configuration;
    }
    else
    {
        PG_FAILSTR("Cannot set the configuration of a mesh generator because the node is already in use");
    }
}

//----------------------------------------------------------------------------------------
    
MeshGenerator::~MeshGenerator()
{
}

//----------------------------------------------------------------------------------------

Graph::NodeData * MeshGenerator::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "MeshGenerator::MeshData", Pegasus::Alloc::PG_MEM_TEMP)
                    MeshData(mConfiguration, GetNodeDataAllocator());
}


}   // namespace Mesh
}   // namespace Pegasus
