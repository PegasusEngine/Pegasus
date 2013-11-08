/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureData.cpp
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture node data, used by all texture nodes, including generators and operators

#include "Pegasus/Texture/TextureData.h"

namespace Pegasus {
namespace Texture {


TextureData::TextureData(const TextureConfiguration & configuration, Alloc::IAllocator* allocator)
:   Graph::NodeData(allocator),
    mConfiguration(configuration)
{
    // Allocate the image data
    const unsigned int imageSize = configuration.GetNumBytes();
    mImageData = PG_NEW_ARRAY(GetAllocator(), -1, "TextureData::mImageData", Alloc::PG_MEM_TEMP, unsigned char, imageSize);
}

//----------------------------------------------------------------------------------------

TextureData::~TextureData()
{
    PG_DELETE_ARRAY(GetAllocator(), mImageData);
}


}   // namespace Texture
}   // namespace Pegasus
