/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureConfiguration.h
//! \author	Kevin Boulanger
//! \date	02nd November 2013
//! \brief	Texture configuration used by all texture nodes, which must be compatible
//!         between nodes to link them

#ifndef PEGASUS_TEXTURE_TEXTURECONFIGURATION_H
#define PEGASUS_TEXTURE_TEXTURECONFIGURATION_H

namespace Pegasus {
namespace Texture {


//! Texture configuration used by all texture nodes, which must be compatible
//! between nodes to link them
class TextureConfiguration
{
public:

    //! Available texture types
    enum Type
    {
        TYPE_1D = 0,        //!< width >= 1, height == depth == 1, numLayers == 1
        TYPE_1D_ARRAY,      //!< width >= 1, height == depth == 1, numLayers == array size
        TYPE_2D,            //!< width >= 1, height >= 1, depth == 1, numLayers == 1
        TYPE_2D_ARRAY,      //!< width >= 1, height >= 1, depth == 1, numLayers == array size
        TYPE_3D,            //!< width >= 1, height >= 1, depth >= 1, numLayers == 1
        TYPE_CUBE,          //!< width >= 1, height >= 1, width == height, depth == 1, numLayers == 6

        NUM_TYPES
    };

    //! Available pixel formats
    enum PixelFormat
    {
        PIXELFORMAT_RGB8 = 0,
        PIXELFORMAT_RGBA8,

        NUM_PIXELFORMATS
    };


    //! Default constructor, sets the resolution to 256x256 and the pixel format to RGB8
    TextureConfiguration();

    //! Constructor
    //! \param type Texture type (TYPE_xxx constant)
    //! \param pixelFormat Pixel format of the texture (PIXELFORMAT_xxx constant)
    //! \param width Horizontal resolution of the texture in pixels (>= 1)
    //! \param height Vertical resolution of the texture in pixels (>= 1)
    //! \param depth Depth of the texture in pixels (>= 1)
    //! \param numLayers Number of layers for array textures, 6 for cube maps, 1 otherwise
    TextureConfiguration(Type type,
                         PixelFormat pixelFormat,
                         unsigned int width,
                         unsigned int height,
                         unsigned int depth,
                         unsigned int numLayers);

    //! Copy constructor
    //! \param other Other configuration to copy from
    explicit TextureConfiguration(const TextureConfiguration & other);

    //! Assignment operator
    //! \param other Other configuration to copy from
    TextureConfiguration & operator=(const TextureConfiguration & other);


    //! Get the type of the texture
    //! \return Type of the texture (TYPE_xxx constant)
    inline Type GetType() const { return mType; }

    //! Get the pixel format of the texture
    //! \return Pixel format of the texture (PIXELFORMAT_xxx constant)
    inline PixelFormat GetPixelFormat() const { return mPixelFormat; }

    //! Get the width of the texture in pixels
    //! \return Horizontal resolution of the texture in pixels (>= 1)
    inline unsigned int GetWidth() const { return mWidth; }

    //! Get the height of the texture in pixels
    //! \return Vertical resolution of the texture in pixels (>= 1)
    inline unsigned int GetHeight() const { return mHeight; }

    //! Get the depth of the texture in pixels
    //! \return Depth of the texture in pixels (>= 1)
    inline unsigned int GetDepth() const { return mDepth; }

    //! Get the number of layers of the texture
    //! \return Number of layers of the texture (>= 1)
    inline unsigned int GetNumLayers() const { return mNumLayers; }


    //! Get the number of bytes per pixel of the texture, computed from the pixel format
    //! \return Number of bytes per pixel of the texture (>= 1)
    unsigned int GetNumBytesPerPixel() const;

    //! Get the number of pixels per layer
    //! \return Number of pixels per layer (>= 1)
    inline unsigned int GetNumPixelsPerLayer() const { return mWidth * mHeight * mDepth; }

    //! Get the number of bytes of the texture for one layer,
    //! computed from the resolution and the pixel format
    //! \return Number of bytes of the texture for one layer (>= 1)
    inline unsigned int GetNumBytesPerLayer() const { return GetNumPixelsPerLayer() * GetNumBytesPerPixel(); }

    //! Get the total number of bytes of the texture
    //! \return Number of bytes of the texture (>= 1)
    inline unsigned int GetNumBytes() const { return mNumLayers * GetNumBytesPerLayer(); }


    //! Test if an input texture configuration is considered as compatible with the current one
    //! \warning This is important to test when linking Texture, TextureGenerator and TextureOperator together
    //! \param configuration Configuration to test with
    //! \return True if the configurations are compatible
    bool IsCompatible(const TextureConfiguration & configuration) const;

    //------------------------------------------------------------------------------------
    
private:

    //! Type of the texture (TextureData::TYPE_xxx constant)
    Type mType;

    //! Pixel format of the texture (TextureData::PIXELFORMAT_xxx constant)
    PixelFormat mPixelFormat;

    //! Horizontal resolution of the texture in pixels (>= 1)
    unsigned int mWidth;

    //! Vertical resolution of the texture in pixels (>= 1)
    unsigned int mHeight;

    //! Depth of the texture in pixels (>= 1)
    unsigned int mDepth;

    // Number of layers for array textures, 6 for cube maps, 1 otherwise
    unsigned int mNumLayers;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTURECONFIGURATION_H
