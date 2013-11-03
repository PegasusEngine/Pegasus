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
    
    //! Available pixel formats
    enum PixelFormat
    {
        PIXELFORMAT_RGB8 = 0,
        PIXELFORMAT_RGBA8,

        NUM_PIXELFORMATS
    };


    //! Constructor
    //! \param width Horizontal resolution of the texture in pixels (>= 1)
    //! \param height Vertical resolution of the texture in pixels (>= 1)
    //! \param pixelFormat Pixel format of the texture (PIXELFORMAT_xxx constant)
    explicit TextureConfiguration(unsigned int width,
                                  unsigned int height,
                                  PixelFormat pixelFormat);

    //! Copy constructor
    //! \param other Other configuration to copy from
    explicit TextureConfiguration(const TextureConfiguration & other);

    //! Assignment operator
    //! \param other Other configuration to copy from
    TextureConfiguration & operator=(const TextureConfiguration & other);


    //! Set the width of the texture in pixels
    //! \param width Horizontal resolution of the texture in pixels (>= 1)
    void SetWidth(unsigned int width);
    
    //! Set the height of the texture in pixels
    //! \param height Vertical resolution of the texture in pixels (>= 1)
    void SetHeight(unsigned int height);

    //! Set the pixel format of the texture
    //! \param pixelFormat Pixel format of the texture (PIXELFORMAT_xxx constant)
    void SetPixelFormat(PixelFormat pixelFormat);


    //! Get the width of the texture in pixels
    //! \return Horizontal resolution of the texture in pixels (>= 1)
    inline unsigned int GetWidth() const { return mWidth; }

    //! Get the height of the texture in pixels
    //! \return Vertical resolution of the texture in pixels (>= 1)
    inline unsigned int GetHeight() const { return mHeight; }

    //! Get the pixel format of the texture
    //! \return Pixel format of the texture (PIXELFORMAT_xxx constant)
    inline PixelFormat GetPixelFormat() const { return mPixelFormat; }

    //! Get the number of bytes per pixel of the texture, computed from the pixel format
    //! \return Number of bytes per pixel of the texture (>= 1)
    unsigned int GetNumBytesPerPixel() const;

    //! Get the number of bytes of the texture, computed from the resolution and the pixel format
    //! \return Number of bytes of the texture (>= 1)
    unsigned int GetNumBytes() const;


    //! Test if an input texture configuration is considered as compatible with the current one
    //! \warning This is important to test when linking Texture, TextureGenerator and TextureOperator together
    //! \param configuration Configuration to test with
    //! \return True if the configurations are compatible
    bool IsCompatible(const TextureConfiguration & configuration) const;

    //------------------------------------------------------------------------------------
    
private:

    //! Horizontal resolution of the texture in pixels (>= 1)
    unsigned int mWidth;

    //! Vertical resolution of the texture in pixels (>= 1)
    unsigned int mHeight;

    //! Pixel format of the texture (TextureData::PIXELFORMAT_xxx constant)
    PixelFormat mPixelFormat;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTURECONFIGURATION_H
