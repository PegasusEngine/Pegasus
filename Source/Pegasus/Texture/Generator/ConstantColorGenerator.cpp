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


BEGIN_IMPLEMENT_PROPERTIES(ConstantColorGenerator)
    IMPLEMENT_PROPERTY(Math::Color8RGBA, Color, Math::Color8RGBA(0, 0, 0, 255))
END_IMPLEMENT_PROPERTIES()

//----------------------------------------------------------------------------------------

void ConstantColorGenerator::GenerateData()
{
    //! \todo Use a simpler syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    const Math::PUInt8 * color8 = GetColor().rgba;
    const Math::PUInt32 color32 = GetColor().rgba32;
    
    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int numBytesPerPixel = configuration.GetNumBytesPerPixel();
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numPixelsPerLayer = configuration.GetNumPixelsPerLayer();
    
    unsigned char * layerData;
    Math::PUInt32 * layerData32;
    unsigned int layer, p;

    // For each layer
    for (layer = 0; layer < numLayers; ++layer)
    {
        layerData = data->GetLayerImageData(layer);

        switch (numBytesPerPixel)
        {
            case 3:
                // For each pixel, copy the constant color
                for (p = 0; p < numPixelsPerLayer; ++p)
                {
                    const unsigned int p3 = p * 3;
                    layerData[p3    ] = color8[0];
                    layerData[p3 + 1] = color8[1];
                    layerData[p3 + 2] = color8[2];
                }
            break;

            case 4:
                // For each pixel, copy the constant color
                layerData32 = reinterpret_cast<Math::PUInt32 *>(layerData);
                for (p = 0; p < numPixelsPerLayer; ++p)
                {
                    layerData32[p] = color32;
                }
            break;

            default:
                PG_FAILSTR("Unsupported number of bytes per pixel (%d) for ConstantColorGenerator", numBytesPerPixel);
        }
    }
}


}   // namespace Texture
}   // namespace Pegasus
