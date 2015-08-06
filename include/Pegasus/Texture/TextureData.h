/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureData.h
//! \author	Karolyn Boulanger
//! \date	01st November 2013
//! \brief	Texture node data, used by all texture nodes, including generators and operators

#ifndef PEGASUS_TEXTURE_TEXTUREDATA_H
#define PEGASUS_TEXTURE_TEXTUREDATA_H

#include "Pegasus/Graph/NodeData.h"
#include "Pegasus/Texture/TextureConfiguration.h"

namespace Pegasus {
namespace Texture {


//! Texture node data, used by all texture nodes, including generators and operators
class TextureData : public Graph::NodeData
{
public:

    //! Default constructor
    //! \param configuration Configuration of the texture, such as the resolution and pixel format
    //! \param allocator Allocator used for the node data
    //! \note Sets the dirty flag
    TextureData(const TextureConfiguration & configuration, Alloc::IAllocator* allocator);

    //! Get the configuration of the texture data
    //! \return Configuration of the texture data, such as the resolution and pixel format
    inline const TextureConfiguration & GetConfiguration() const { return mConfiguration; }

    //! Get the image data for a layer
    //! \param layer Index of the layer (< mNumLayers)
    inline unsigned char * GetLayerImageData(unsigned int layer)
        {
            PG_ASSERTSTR(layer < mConfiguration.GetNumLayers(), "Invalid layer index (%d), it must be < %d", layer, mConfiguration.GetNumLayers());
            return mImageData[layer];
        }

    //! Get the image data for a layer (const version)
    //! \param layer Index of the layer (< mNumLayers)
    inline const unsigned char * GetLayerImageData(unsigned int layer) const
        {
            PG_ASSERTSTR(layer < mConfiguration.GetNumLayers(), "Invalid layer index (%d), it must be < %d", layer, mConfiguration.GetNumLayers());
            return mImageData[layer];
        }

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~TextureData();

    //------------------------------------------------------------------------------------
    
private:

    // Node data cannot be copied, only references to them
    PG_DISABLE_COPY(TextureData)

    //! Configuration of the texture data, such as the resolution and pixel format
    TextureConfiguration mConfiguration;


    //! Image data of the texture, never nullptr.
    //! mImageData[layer][z*height*width + y*height + x]
    unsigned char ** mImageData;
};

//----------------------------------------------------------------------------------------

//! Reference to a TextureData, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TextureData>   TextureDataRef;

//! Const reference to a reference to a TextureData, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TextureData> & TextureDataIn;

//! Reference to a reference to a TextureData, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TextureData> & TextureDataInOut;

//! Reference to a TextureData, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TextureData>   TextureDataReturn;


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREDATA_H
