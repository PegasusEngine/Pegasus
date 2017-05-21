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
#include "Pegasus/Mesh/IMeshFactory.h"

namespace Pegasus {
namespace Mesh {


BEGIN_IMPLEMENT_PROPERTIES(MeshGenerator)
END_IMPLEMENT_PROPERTIES(MeshGenerator)

//----------------------------------------------------------------------------------------

MeshGenerator::MeshGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mFactory(nullptr),
    mConfiguration()
{
    BEGIN_INIT_PROPERTIES(MeshGenerator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER

    //default mesh editor layout.
    mConfiguration.GetInputLayout()->GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV | MeshInputLayout::USE_NORMAL);
}

//----------------------------------------------------------------------------------------

MeshGenerator::MeshGenerator(const MeshConfiguration & configuration,
                                   Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::GeneratorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
    BEGIN_INIT_PROPERTIES(MeshGenerator)
    END_INIT_PROPERTIES()

    // Initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
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
    ReleaseGPUData();
}

//----------------------------------------------------------------------------------------

void MeshGenerator::ReleaseGPUData()
{
    if (GetData() != nullptr)
    {
        GetFactory()->DestroyNodeGPUData((MeshData*)&(*GetData()));
    }
}

//----------------------------------------------------------------------------------------

void MeshGenerator::ReleaseDataAndPropagate()
{
    //! \todo See note in ReleaseGPUData()
    ReleaseGPUData();

    Graph::Node::ReleaseDataAndPropagate();
}

//----------------------------------------------------------------------------------------

Graph::NodeData * MeshGenerator::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "MeshGenerator::MeshData", Pegasus::Alloc::PG_MEM_TEMP)
                    MeshData(mConfiguration, GetMode(), GetNodeDataAllocator());
}


}   // namespace Mesh
}   // namespace Pegasus
