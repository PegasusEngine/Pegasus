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

    //! \todo Compute the color constant from the properties
    /*const*/ unsigned char color[4] = { 132, 5, 212, 255 };
    static bool firstTime = true;
    if (!firstTime)
    {
        color[0] = 212;
        color[2] = 132;
    }
    firstTime = false;

    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int numBytesPerPixel = configuration.GetNumBytesPerPixel();
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numPixelsPerLayer = configuration.GetNumPixelsPerLayer();
    
    unsigned int layer, p;
    unsigned char * layerData;

    // For each layer
    switch (numBytesPerPixel)
    {
        case 3:
            for (layer = 0; layer < numLayers; ++layer)
            {
                layerData = data->GetLayerImageData(layer);
        
                // For each pixel, copy the constant color
                for (p = 0; p < numPixelsPerLayer; ++p)
                {
                    const unsigned int p3 = p * 3;
                    layerData[p3    ] = color[0];
                    layerData[p3 + 1] = color[1];
                    layerData[p3 + 2] = color[2];
                }
            }
            break;

        case 4:
            for (layer = 0; layer < numLayers; ++layer)
            {
                layerData = data->GetLayerImageData(layer);
        
                // For each pixel, copy the constant color
                for (p = 0; p < numPixelsPerLayer; ++p)
                {
                    const unsigned int p4 = p << 2;
                    layerData[p4    ] = color[0];
                    layerData[p4 + 1] = color[1];
                    layerData[p4 + 2] = color[2];
                    layerData[p4 + 3] = color[3];
                }
            }
            break;

        default:
            PG_FAILSTR("Unsupported number of bytes per pixel (%d) for ConstantColorGenerator", numBytesPerPixel);
    }
}


}   // namespace Texture
}   // namespace Pegasus
