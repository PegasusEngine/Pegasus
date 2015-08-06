/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	AddOperator.cpp
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture operator that adds an arbitrary number of textures together

#include "Pegasus/Texture/Shared/TextureEvent.h"
#include "Pegasus/Texture/Operator/AddOperator.h"
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(AddOperator)
    IMPLEMENT_PROPERTY(bool, Clamp, true)
END_IMPLEMENT_PROPERTIES()

//----------------------------------------------------------------------------------------

void AddOperator::GenerateData()
{
    GRAPH_EVENT_DISPATCH(this, TextureOperationEvent, TextureOperationEvent::BEGIN);

    //! \todo Use a simpler syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int numLayers = configuration.GetNumLayers();
    const unsigned int numBytesPerLayer = configuration.GetNumBytesPerLayer();

    unsigned char * layerData;
    const TextureData * inputData;
    const unsigned char * inputLayerData;
    unsigned int layer, b;
    unsigned short addedValue;
    bool updated;

    // Copy the first input texture
    for (layer = 0; layer < numLayers; ++layer)
    {
        //! \todo Use a simpler syntax
        updated = false;
        inputData = static_cast<TextureData *>(&(*GetInput(0)->GetUpdatedData(updated)));
        inputLayerData = inputData->GetLayerImageData(layer);
        layerData = data->GetLayerImageData(layer);

        Utils::Memcpy(layerData, inputLayerData, numBytesPerLayer);
    }

    // For each extra input texture to add
    const unsigned int numInputs = GetNumInputs();
    unsigned int input;
    for (input = 1; input < numInputs; ++input)
    {
        //! \todo Use a simpler syntax
        updated = false;
        inputData = static_cast<TextureData *>(&(*GetInput(input)->GetUpdatedData(updated)));

        for (layer = 0; layer < numLayers; ++layer)
        {
            inputLayerData = inputData->GetLayerImageData(layer);
            layerData = data->GetLayerImageData(layer);

            if (GetClamp())
            {
                // For each component of each pixel, perform the addition
                for (b = 0; b < numBytesPerLayer; ++b)
                {
                    // Add the values and clamp
                    addedValue = static_cast<unsigned short>(layerData[b]) + static_cast<unsigned short>(inputLayerData[b]);
                    if (addedValue > 255)
                    {
                        addedValue = 255;
                    }
                    layerData[b] = static_cast<unsigned char>(addedValue);
                }
            }
            else
            {
                // For each component of each pixel, perform the addition
                for (b = 0; b < numBytesPerLayer; ++b)
                {
                    // Cannot use += on unsigned chars with no masking.
                    // When overflowing, the runtime can detect the loss of data
                    layerData[b] = static_cast<unsigned char>((layerData[b] + inputLayerData[b]) & 0xFF);
                }
            }
        }
    }

    GRAPH_EVENT_DISPATCH(this, TextureOperationEvent, TextureOperationEvent::END_SUCCESS);
}


}   // namespace Texture
}   // namespace Pegasus
