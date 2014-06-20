/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BoxGenerator.h
//! \author	Kleber Garcia
//! \date	June 16th 2014
//! \brief	Cube Generator

#ifndef PEGASUS_CUBE_GENERATOR_H
#define PEGASUS_CUBE_GENERATOR_H

#include "Pegasus/Mesh/MeshGenerator.h"

namespace Pegasus
{

namespace Mesh
{

//! Mesh cube generator. Generates a cube
class BoxGenerator : public MeshGenerator
{
public:
    DECLARE_MESH_GENERATOR_NODE(BoxGenerator)

    //! Cube generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    BoxGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~BoxGenerator();

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};
}

}

#endif//PEGASUS_CUBE_GENERATOR_H
