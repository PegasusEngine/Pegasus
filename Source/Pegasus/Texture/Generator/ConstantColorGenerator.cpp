/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConstantColorGenerator.cpp
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture generator that fills the image with a constant color

#include "Pegasus/Texture/Generator/ConstantColorGenerator.h"
#include "Pegasus/Math/Types.h"
#include "Pegasus/Utils/Memset.h"

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(ConstantColorGenerator)
    IMPLEMENT_PROPERTY(ConstantColorGenerator, Color)
END_IMPLEMENT_PROPERTIES(ConstantColorGenerator)



//----------------------------------------------------------------------------------------

void ConstantColorGenerator::InitProperties()
{
    BEGIN_INIT_PROPERTIES(ConstantColorGenerator)
        INIT_PROPERTY(Color)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

void ConstantColorGenerator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::BEGIN);

    //! \todo Use a simpler syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    const Math::PUInt32 color32 = GetColor().rgba32;
    
    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int numBytesPerPixel = configuration.GetNumBytesPerPixel();
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numBytesPerLayer = configuration.GetNumBytesPerLayer();
    
    unsigned char * layerData;
    unsigned int layer;

    // For each layer
    for (layer = 0; layer < numLayers; ++layer)
    {
        layerData = data->GetLayerImageData(layer);

        switch (numBytesPerPixel)
        {
            case 4:
                // For each pixel, copy the constant color
                Utils::Memset32(layerData, color32, numBytesPerLayer);
                break;

            default:
                PG_FAILSTR("Unsupported number of bytes per pixel (%d) for ConstantColorGenerator", numBytesPerPixel);
        }
    }

    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::END_SUCCESS);
}


}   // namespace Texture
}   // namespace Pegasus
