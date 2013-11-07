/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConstantColorGenerator.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture generator that fills the image with a constant color

#include "Pegasus/Texture/Generator/ConstantColorGenerator.h"

namespace Pegasus {
namespace Texture {


void ConstantColorGenerator::GenerateData()
{
    //! \todo Use an easier syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    //! \todo Fill the data
}


}   // namespace Texture
}   // namespace Pegasus
