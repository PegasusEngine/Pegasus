/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Mesh.h	
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh output node

#ifndef PEGASUS_MESH_MESH_H
#define PEGASUS_MESH_MESH_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/OutputNode.h"
#include "Pegasus/Mesh/MeshConfiguration.h"
#include "Pegasus/Mesh/MeshData.h"
#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Mesh/MeshOperator.h"

namespace Pegasus {
namespace Mesh {

class IMeshFactory;

//! Base output node class, for the root of the graphs.
//! \warning Has one and only one input node, operator or generator
//! \note Does not own an instance of NodeData, it only redirects the data of its input
//!       or one of its inputs
class Mesh : public Graph::OutputNode
{
public:

    //! Default constructor, uses the default mesh configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    Mesh(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);


    //! Get the configuration of the mesh
    //! \return Configuration of the mesh
    inline const MeshConfiguration & GetConfiguration() const { return mConfiguration; }

    //! Sets the GPU factory for the mesh
    void SetFactory(IMeshFactory * factory) { mFactory = factory; }

    //! Set the input of the mesh output node to a generator node
    //! \warning If an input was already set (generator or operator), it is replaced
    //! \note If the generator is incompatible with the current node,
    //!       an event thrown, and nothing is replaced
    //! \param meshGenerator Generator node to use as input
    void SetGeneratorInput(MeshGeneratorIn meshGenerator);

    //! Set the input of the mesh output node to a operator node
    //! \warning If an input was already set (generator or operator), it is replaced
    //! \note If the operator is incompatible with the current node,
    //!       an event is thrown, and nothing is replaced
    //! \param meshOperator Operator node to use as input
    void SetOperatorInput(MeshOperatorIn meshOperator);


    //! Return the mesh up-to-date data.
    //! \return Reference to the mesh data, belonging either to the current node
    //!         or to one of the input nodes, cannot be a null reference.
    //! \note Calls GetUpdatedData() internally, and regenerate the mesh data
    //!       if any part of the graph is dirty
    virtual MeshDataReturn GetUpdatedMeshData();


    //! Releases the node internal data
    void ReleaseDataAndPropagate();


    //! Mesh node creation function, used by the mesh manager
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    //! \return New instance of the mesh node
    static Graph::NodeReturn CreateNode(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
    {   return PG_NEW(nodeAllocator, -1, "Mesh", Alloc::PG_MEM_PERM) Mesh(nodeAllocator, nodeDataAllocator); }

    //------------------------------------------------------------------------------------

protected:

    //! Destructor
    virtual ~Mesh();

    //! Set the configuration of the mesh
    //! \warning Can be done only after the constructor has been called, when no input node is connected yet.
    //!          In case of error, the configuration is not set
    void SetConfiguration(const MeshConfiguration & configuration);

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(Mesh)

    //! Configuration of the mesh, such as the resolution and pixel format
    MeshConfiguration mConfiguration;

    //! Pointer to GPU Mesh factory
    IMeshFactory * mFactory;
};

//----------------------------------------------------------------------------------------

//! Reference to a Mesh, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<Mesh>   MeshRef;

//! Const reference to a reference to a Mesh, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<Mesh> & MeshIn;

//! Reference to a reference to a Mesh, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<Mesh> & MeshInOut;

//! Reference to a Mesh, typically used as the return value of a function
typedef       Pegasus::Core::Ref<Mesh>   MeshReturn;


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESH_H
