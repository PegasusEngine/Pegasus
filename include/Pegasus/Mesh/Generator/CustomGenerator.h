/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CustomGenerator.h
//! \author	Kleber Garcia
//! \date	September 7th 2016
//! \brief	Custom generator

#ifndef PEGASUS_GRID_GENERATOR_H
#define PEGASUS_GRID_GENERATOR_H

#include "Pegasus/Mesh/MeshGenerator.h"

namespace Pegasus
{

namespace Mesh
{

class CustomGenerator : public MeshGenerator
{
    DECLARE_MESH_GENERATOR_NODE(CustomGenerator)

public:

    //! Cube generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    CustomGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~CustomGenerator();

    MeshDataRef EditMeshData();

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};
}

}

#endif//PEGASUS_CUBE_GENERATOR_H
