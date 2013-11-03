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


TextureData::TextureData(const TextureConfiguration & configuration)
:   Graph::NodeData(),
    mConfiguration(configuration)
{
    // Allocate the image data
    //! \todo Use an allocator
    const unsigned int imageSize = configuration.GetNumBytes();
    mImageData = PG_NEW_ARRAY("TextureData::mImageData", Memory::PG_MEM_PERM, unsigned char, imageSize);
}

//----------------------------------------------------------------------------------------

TextureData::~TextureData()
{
    PG_DELETE_ARRAY mImageData;
}


}   // namespace Texture
}   // namespace Pegasus
