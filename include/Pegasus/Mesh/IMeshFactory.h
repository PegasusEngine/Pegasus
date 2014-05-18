/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IMeshFactory.h
//! \author Kleber Garcia
//! \date   16th May 2014
//! \brief  Major factory interface which creates mesh GPU data.

#ifndef PEGASUS_IMESHFACTORY_H
#define PEGASUS_IMESHFACTORY_H


namespace Pegasus
{

namespace Alloc {
    class IAllocator;
}

namespace Graph {
    class NodeData;
}

namespace Mesh {

    //forward declarations
    class MeshData;


    //! Mesh GPU factory interface. Sets GPU data for a shader node.
    class IMeshFactory
    {
    public:
        IMeshFactory() {}
        virtual ~IMeshFactory() {}

        //! callback to be called at application constructor. Sets the right allocators for the mesh GPU data.
        //! \param allocator the allocator to set
        virtual void Initialize(Pegasus::Alloc::IAllocator * allocator) = 0;

        //! Generates GPU data for a mesh data node. 
        //! \param nodeData 
        virtual void GenerateMeshGPUData(MeshData * nodeData) = 0;

        //! Destroys GPU data for a mesh data node. 
        //! \param nodeData 
        virtual void DestroyNodeGPUData(MeshData * nodeData) = 0;
    };
}


}

#endif //PEGASUS_IMESHFACTORY_H
