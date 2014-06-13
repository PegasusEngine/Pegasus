/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TextureFactory.h
//! \author Kleber Garcia
//! \date   1st June 2014
//! \brief  Factory creator functions. These function pointers will return factories for
//!         the node classes to be used in the conversion of GPU to CPU data
#ifndef PEGASUS_RENDER_TEXTURE_FACTORY_H
#define PEGASUS_RENDER_TEXTURE_FACTORY_H

#include "Pegasus/Texture/ITextureFactory.h"

namespace Pegasus {
namespace Render {

//! \return returns a texture factory implementation.
//!         this implementation needs to be passed to the TextureManager
Pegasus::Texture::ITextureFactory * GetRenderTextureFactory();

}
}

#endif
