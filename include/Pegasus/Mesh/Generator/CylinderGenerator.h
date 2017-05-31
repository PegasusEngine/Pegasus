/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CylinderGenerator.h
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh generator of a quad 

#ifndef PEGASUS_MESH_GENERATOR_CYLINDER_H
#define PEGASUS_MESH_GENERATOR_CYLINDER

#include "Pegasus/Mesh/MeshGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace Mesh
{

//! Mesh quad generator. Generates a quad in homogeneous space [-1, 1]. Sets the z of such quad to 0
class CylinderGenerator : public MeshGenerator
{
    DECLARE_MESH_GENERATOR_NODE(CylinderGenerator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(CylinderGenerator, MeshGenerator)
        DECLARE_PROPERTY(float, CylinderCapRadius, 1.0f)
        DECLARE_PROPERTY(float, CylinderHeight,    5.0f)
        DECLARE_PROPERTY(int,   CylinderRingCuts, 0)
        DECLARE_PROPERTY(int,   CylinderFaceCount, 8)
    END_DECLARE_PROPERTIES()

public:

    //!Quad generator constructor
    //!\param nodeAllocator the allocator for the node properties (if any)
    //!\param nodeDataAllocator the allocator of the node data
    CylinderGenerator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~CylinderGenerator();

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

    void CreateRing(
        StdVertex* destination,
        int destinationOffset,
        float zVal,
        int faceCount,
        unsigned short* indexBuffer,
        int& nextIndex,
        const Math::Vec2& uvOffset,
        const Math::Vec2& scale,
        bool isCap,
        bool isLowerCap);
};

} //Pegasus
} //Mesh

#endif
