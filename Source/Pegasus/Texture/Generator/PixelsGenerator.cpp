/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PixelsGenerator.cpp
//! \author	Karolyn Boulanger
//! \date	17th June 2013
//! \brief	Texture generator that renders randomly located pixels

#include "Pegasus/Texture/Generator/PixelsGenerator.h"
#include "Pegasus/Math/Types.h"
#include "Pegasus/Utils/Memset.h"
#include <stdlib.h>

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(PixelsGenerator)
    IMPLEMENT_PROPERTY(PixelsGenerator, NumPixels)
    IMPLEMENT_PROPERTY(PixelsGenerator, Seed)
    IMPLEMENT_PROPERTY(PixelsGenerator, BackgroundColor)
    IMPLEMENT_PROPERTY(PixelsGenerator, Color0)
END_IMPLEMENT_PROPERTIES(PixelsGenerator)

//----------------------------------------------------------------------------------------

namespace Internal {

//! \todo Implement PG_STATIC_ASSERT
// PG_STATIC_ASSERT(RAND_MAX == 0x7FFF);

//! Number of bits to shift to have a division by RAND_MAX + 1
static const unsigned int RAND_MAX_SHIFT = 15;

//! Choose a random pixel in the texture, given its dimensions
//! \param width  Width of the texture in pixels  (0 < width  < 65536)
//! \param height Height of the texture in pixels (0 < height < 65536)
//! \param depth  Depth of the texture in pixels  (0 < depth  < 65536)
//! \return Offset into the linear texture, in pixels
static inline unsigned int ChooseRandomPixel(unsigned int width,
                                             unsigned int height,
                                             unsigned int depth)
{
    const unsigned int px = (static_cast<unsigned int>(rand()) * width ) >> RAND_MAX_SHIFT;
    const unsigned int py = (static_cast<unsigned int>(rand()) * height) >> RAND_MAX_SHIFT;
    const unsigned int pz = (static_cast<unsigned int>(rand()) * depth ) >> RAND_MAX_SHIFT;

    return (pz * height + py) * width + px;
}

}   // namespace Internal

//----------------------------------------------------------------------------------------

void PixelsGenerator::InitProperties()
{
    BEGIN_INIT_PROPERTIES(PixelsGenerator)
        INIT_PROPERTY(NumPixels)
        INIT_PROPERTY(Seed)
        INIT_PROPERTY(BackgroundColor)
        INIT_PROPERTY(Color0)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

void PixelsGenerator::GenerateData()
{
    //! \todo Could it be placed in the caller instead?
    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::BEGIN);

    //! \todo Use a simpler syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    const Math::PUInt32 backColor32 = GetBackgroundColor().rgba32;
    const Math::PUInt32 color0_32 = GetColor0().rgba32;
    
    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int width  = configuration.GetWidth ();
    const unsigned int height = configuration.GetHeight();
    const unsigned int depth  = configuration.GetDepth ();
    const unsigned int numBytesPerPixel = configuration.GetNumBytesPerPixel();
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numBytesPerLayer = configuration.GetNumBytesPerLayer();

    const unsigned int numPixelsToRender = GetNumPixels();

    // Initialize the random number generator
    srand(GetSeed());

    unsigned char * layerData;
    Math::PUInt32 * layerData32;
    unsigned int layer, p;

    // For each layer
    for (layer = 0; layer < numLayers; ++layer)
    {
        layerData = data->GetLayerImageData(layer);

        switch (numBytesPerPixel)
        {
            case 4:
                // For each background pixel, copy the background color
                layerData32 = reinterpret_cast<Math::PUInt32 *>(layerData);
                Utils::Memset32(layerData32, backColor32, numBytesPerLayer);

                // For each random pixel
                for (p = 0; p < numPixelsToRender; ++p)
                {
                    layerData32[Internal::ChooseRandomPixel(width, height, depth)] = color0_32;
                }
            break;

            default:
                PG_FAILSTR("Unsupported number of bytes per pixel (%d) for PixelsGenerator", numBytesPerPixel);
        }
    }

    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::END_SUCCESS);
}


}   // namespace Texture
}   // namespace Pegasus
