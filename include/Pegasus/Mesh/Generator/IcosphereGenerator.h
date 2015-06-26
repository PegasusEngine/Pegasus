/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IcosphereGenerator.h
//! \author	Kleber Garcia
//! \date	June 29th 2014
//! \brief	Icosphere Generator

#ifndef PEGASUS_ICOSPHERE_GENERATOR_H
#define PEGASUS_ICOSPHERE_GENERATOR_H

#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Utils/TesselationTable.h"

namespace Pegasus
{
namespace Mesh
{

class IcosphereGenerator : public MeshGenerator
{
public:
    DECLARE_MESH_GENERATOR_NODE(IcosphereGenerator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES2(IcosphereGenerator, MeshGenerator)
        DECLARE_PROPERTY2(int,   Degree, 3)
        DECLARE_PROPERTY2(float, Radius, 1)
    END_DECLARE_PROPERTIES2()

    //! Icosphere generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data

    IcosphereGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);
    
    virtual ~IcosphereGenerator();

protected:

    //! Generate the content of the data associated with the mesh generator
    virtual void GenerateData();

    //! generates the midpoint between two vertices (passed by index). Caches the index
    //! and returns the cached one if is generated.
    //! \param p1 the first parent
    //! \param p2 the second parent
    //! \return the new child index
    unsigned short GenChild(MeshData * meshData, unsigned short p1, unsigned short p2);

    //! recursive function that tesselates the icosphere
    void Tesselate(MeshData * meshData, int level, unsigned short a, unsigned short b, unsigned short c);

    Utils::TesselationTable mIdxCache;
       
};

}
}

#endif //PEGASUS_ICOSPHERE_GENERATOR
