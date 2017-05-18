/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshOperator.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Base mesh operator node class

#ifndef PEGASUS_MESH_MESHOPERATOR_H
#define PEGASUS_MESH_MESHOPERATOR_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/OperatorNode.h"
#include "Pegasus/Mesh/Shared/MeshEvent.h"
#include "Pegasus/Mesh/MeshConfiguration.h"
#include "Pegasus/Mesh/MeshData.h"
#include "Pegasus/Mesh/MeshDeclaration.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus {
namespace Mesh {

class IMeshFactory;

//! Base mesh operator node class
//! \warning IMPORTANT! When deriving from this class, update MeshManager::RegisterAllMeshNodes()
//!                     so the operator node can be instantiated
class MeshOperator : public Graph::OperatorNode
{
    BEGIN_DECLARE_PROPERTIES_BASE(MeshOperator)
    END_DECLARE_PROPERTIES()

    PEGASUS_EVENT_DECLARE_DISPATCHER(IMeshEventListener);

public:

    //! Default constructor, uses the default mesh configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    MeshOperator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \param configuration Configuration of the operator
    MeshOperator(const MeshConfiguration & configuration,
                    Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Set the configuration of the mesh operator
    //! \warning Can be done only after the constructor has been called
    void SetConfiguration(const MeshConfiguration & configuration);

    //! Get the configuration of the operator
    //! \return Configuration of the operator
    inline const MeshConfiguration & GetConfiguration() const { return mConfiguration; }

    //! Sets the GPU factory for the mesh
    void SetFactory(IMeshFactory * factory) { mFactory = factory; }

    //! Return the mesh operator up-to-date data.
    //! \note Defines the standard behavior of all mesh operator nodes.
    //!       Calls GetUpdatedData() on all inputs, and if any of them was dirty,
    //!       then calls GenerateData() to update the node data.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node or any of its input nodes has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note When asking for the data of a node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------

protected:

    //! Destructor
    virtual ~MeshOperator();

    //! Allocate the data associated with the mesh operator
    //! \warning Do not override in derived classes since all operators
    //!          use the same mesh data class
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const;

    //! Gets the GPU factory for the mesh
    IMeshFactory* GetFactory() { return mFactory; }

    //! Generate the content of the data associated with the mesh operator
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //! Releases the node internal data
    void ReleaseGPUData();

    //! Releases the node internal data
    virtual void ReleaseDataAndPropagate();

    //------------------------------------------------------------------------------------

    //! Configuration of the operator, such as the resolution and pixel format
    MeshConfiguration mConfiguration;

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(MeshOperator)


    //! Pointer to GPU Mesh factory
    IMeshFactory * mFactory;
};

//----------------------------------------------------------------------------------------

//! Reference to a MeshOperator, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<MeshOperator>   MeshOperatorRef;

//! Const reference to a reference to a MeshOperator, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<MeshOperator> & MeshOperatorIn;

//! Reference to a reference to a MeshOperator, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<MeshOperator> & MeshOperatorInOut;

//! Reference to a MeshOperator, typically used as the return value of a function
typedef       Pegasus::Core::Ref<MeshOperator>   MeshOperatorReturn;


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHOPERATOR_H
