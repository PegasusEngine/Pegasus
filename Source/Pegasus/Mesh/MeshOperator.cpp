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
#include "Pegasus/Mesh/IMeshFactory.h"

namespace Pegasus {
namespace Mesh {


BEGIN_IMPLEMENT_PROPERTIES(MeshOperator)
END_IMPLEMENT_PROPERTIES(MeshOperator)

//----------------------------------------------------------------------------------------

MeshOperator::MeshOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mFactory(nullptr),
    mConfiguration()
{
    BEGIN_INIT_PROPERTIES(MeshOperator)
    END_INIT_PROPERTIES()

    //initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER

    //default mesh editor layout.
    MeshInputLayout editorIL;
    editorIL.GenerateEditorLayout(MeshInputLayout::USE_POSITION | MeshInputLayout::USE_UV | MeshInputLayout::USE_NORMAL);
    mConfiguration.SetInputLayout(editorIL);
}

//----------------------------------------------------------------------------------------

MeshOperator::MeshOperator(const MeshConfiguration & configuration,
                                 Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
:   Graph::OperatorNode(nodeAllocator, nodeDataAllocator),
    mConfiguration(configuration)
{
    //initialize event user data
    PEGASUS_EVENT_INIT_DISPATCHER
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
    ReleaseGPUData();
}

//----------------------------------------------------------------------------------------

void MeshOperator::ReleaseGPUData()
{
    if (GetData() != nullptr)
    {
        GetFactory()->DestroyNodeGPUData((MeshData*)&(*GetData()));
    }
}

//----------------------------------------------------------------------------------------

void MeshOperator::ReleaseDataAndPropagate()
{
    //! \todo See note in ReleaseGPUData()
    ReleaseGPUData();

    Graph::Node::ReleaseDataAndPropagate();
}

//----------------------------------------------------------------------------------------

Graph::NodeData * MeshOperator::AllocateData() const
{
    return PG_NEW(GetNodeDataAllocator(), -1, "MeshOperator::MeshData", Pegasus::Alloc::PG_MEM_TEMP)
                    MeshData(mConfiguration, GetMode(), GetNodeDataAllocator());
}

//----------------------------------------------------------------------------------------

void MeshOperator::AddGeneratorInput(MeshGeneratorIn meshGenerator)
{
    if (meshGenerator->GetConfiguration() == GetConfiguration())
    {
        AddInput(meshGenerator);
    }
    else
    {
        PG_LOG('ERR_', "Cannot connect mesh generator to mesh operator since configurations vary.");
    }
}

//----------------------------------------------------------------------------------------

void MeshOperator::AddOperatorInput(MeshOperatorIn meshOperator)
{
    if (meshOperator->GetConfiguration() == GetConfiguration())
    {
        AddInput(meshOperator);
    }
    else
    {
        PG_LOG('ERR_', "Cannot connect mesh operator to mesh operator since configurations vary.");
    }
}

}   // namespace Mesh
}   // namespace Pegasus
