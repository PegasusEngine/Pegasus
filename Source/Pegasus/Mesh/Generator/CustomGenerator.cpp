/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CustomGenerator.cpp
//! \author	Kleber Garcia
//! \date	September 7th 2016
//! \brief	Custom generator

#include "Pegasus/Mesh/Generator/CustomGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Mesh {

//----------------------------------------------------------------------------------------

CustomGenerator::CustomGenerator(Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
: MeshGenerator(nodeAllocator, nodeDataAllocator)
{
}

CustomGenerator::~CustomGenerator()
{
}

MeshDataRef CustomGenerator::EditMeshData()
{
    bool dummy = false;
    MeshDataRef meshData = GetUpdatedData(dummy);
    meshData->Invalidate();
    return meshData;
}

//----------------------------------------------------------------------------------------

void CustomGenerator::GenerateData()
{
   
}


}
}
