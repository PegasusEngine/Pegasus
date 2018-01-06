/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CustomGenerator.cpp
//! \author	Kleber Garcia
//! \date	January 9th 2018
//! \brief	Custom generator

#include "Pegasus/Texture/Generator/TexCustomGenerator.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus {
namespace Texture {

TextureDataRef TexCustomGenerator::EditTextureData()
{
    bool dummy = false;
    TextureDataRef texData = GetUpdatedData(dummy);
    texData->Invalidate();
    return texData;
}

void TexCustomGenerator::InitProperties()
{
}

void TexCustomGenerator::GenerateData()
{   
}


}
}
