/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	QuadGenerator.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh generator of a quad 

#ifndef PEGASUS_MESH_GENERATOR_MESHGENERATOR_H
#define PEGASUS_MESH_GENERATOR_MESHGENERATOR_H

#include "Pegasus/Mesh/MeshGenerator.h"

namespace Pegasus
{
namespace Mesh
{

//! Mesh quad generator. Generates a quad in homogeneous space [-1, 1]. Sets the z of such quad to 0
class QuadGenerator : public MeshGenerator
{
public:
    DECLARE_MESH_GENERATOR_NODE(QuadGenerator)

    //!Quad generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    QuadGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~QuadGenerator();

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();
};

} //Pegasus
} //Mesh

#endif
