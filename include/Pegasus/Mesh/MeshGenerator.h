/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshGenerator.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Base mesh generator node class

#ifndef PEGASUS_MESH_MESHGENERATOR_H
#define PEGASUS_MESH_MESHGENERATOR_H

#include "Pegasus/Core/Ref.h"
#include "Pegasus/Graph/GeneratorNode.h"
#include "Pegasus/Mesh/MeshConfiguration.h"
#include "Pegasus/Mesh/MeshData.h"
#include "Pegasus/Mesh/MeshDeclaration.h"

namespace Pegasus {
namespace Mesh {


//! Base mesh generator node class
//! \warning IMPORTANT! When deriving from this class, update MeshManager::RegisterAllMeshNodes()
//!                     so the generator node can be instantiated
class MeshGenerator : public Graph::GeneratorNode
{
public:

    //! Default constructor, uses the default mesh configuration
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    MeshGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Constructor
    //! \param configuration Configuration of the generator
    //! \param nodeAllocator Allocator used for node internal data (except the attached NodeData)
    //! \param nodeDataAllocator Allocator used for NodeData
    MeshGenerator(const MeshConfiguration & configuration,
                     Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator);

    //! Set the configuration of the mesh generator
    //! \warning Can be done only after the constructor has been called
    void SetConfiguration(const MeshConfiguration & configuration);

    //! Get the configuration of the generator
    //! \return Configuration of the generator
    inline const MeshConfiguration & GetConfiguration() const { return mConfiguration; }


    //! Return the mesh generator up-to-date data.
    //! \note Defines the standard behavior of all generator nodes.
    //!       Calls GenerateData() if the node data is dirty.
    //!       It should be overridden only for special cases.
    //! \param updated Set to true if the node has had the data recomputed
    //!                (output parameter, set to false only by the caller)
    //! \return Reference to the node data, cannot be a null reference.
    //! \note When asking for the data of a generator node, it needs to be allocated and updated
    //!       to not have the dirty flag turned on.
    //!       Redefine this function in derived classes to change its behavior
    //virtual NodeDataReturn GetUpdatedData(bool & updated);

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~MeshGenerator();

    //! Allocate the data associated with the mesh generator
    //! \warning Do not override in derived classes since all generators
    //!          use the same mesh data class
    //! \warning Do not update mData internally, just return the pointer to the data
    //! \note Called by CreateData()
    //! \return Pointer to the data being allocated
    virtual Graph::NodeData * AllocateData() const;


    //! Generate the content of the data associated with the mesh generator
    //! \warning To be redefined by each derived class, to implement its behavior
    //! \note Called by \a GetUpdatedData()
    virtual void GenerateData() = 0;

    //! Configuration of the generator
    MeshConfiguration mConfiguration;

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(MeshGenerator)

};

//----------------------------------------------------------------------------------------

//! Reference to a MeshGenerator, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<MeshGenerator>   MeshGeneratorRef;

//! Const reference to a reference to a MeshGenerator, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<MeshGenerator> & MeshGeneratorIn;

//! Reference to a reference to a MeshGenerator, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<MeshGenerator> & MeshGeneratorInOut;

//! Reference to a MeshGenerator, typically used as the return value of a function
typedef       Pegasus::Core::Ref<MeshGenerator>   MeshGeneratorReturn;


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHGENERATOR_H
