/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureData.cpp
//! \author	Karolyn Boulanger
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
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numBytesPerLayer = configuration.GetNumBytesPerLayer();
    mImageData = PG_NEW_ARRAY(GetAllocator(), -1, "TextureData::mImageData", Alloc::PG_MEM_TEMP, unsigned char *, numLayers);
    for (unsigned int layer = 0; layer < numLayers; ++layer)
    {
        mImageData[layer] = PG_NEW_ARRAY(GetAllocator(), -1, "TextureData::mImageData[layer]", Alloc::PG_MEM_TEMP, unsigned char, numBytesPerLayer);
    }
}

//----------------------------------------------------------------------------------------

TextureData::~TextureData()
{
    const unsigned int numLayers = mConfiguration.GetNumLayers();
    for (unsigned int layer = 0; layer < numLayers; ++layer)
    {
        PG_DELETE_ARRAY(GetAllocator(), mImageData[layer]);
    }
    PG_DELETE_ARRAY(GetAllocator(), mImageData);
}


}   // namespace Texture
}   // namespace Pegasus
