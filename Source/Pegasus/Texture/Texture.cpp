/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Texture.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture output node

#include "Pegasus/Texture/Texture.h"

namespace Pegasus {
namespace Texture {


Texture::Texture(const TextureConfiguration & configuration)
:   Graph::OutputNode(),
    mConfiguration(configuration)
{
}

//----------------------------------------------------------------------------------------

//void Texture::SetGeneratorInput(TextureGeneratorIn generator)
//{
//}

//----------------------------------------------------------------------------------------

//void Texture::SetOperatorInput(TextureOperatorIn generator)
//{
//}

//----------------------------------------------------------------------------------------

Texture::~Texture()
{
}


}   // namespace Texture
}   // namespace Pegasus
