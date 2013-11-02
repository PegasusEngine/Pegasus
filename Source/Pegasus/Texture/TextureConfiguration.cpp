/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureConfiguration.cpp
//! \author	Kevin Boulanger
//! \date	02nd November 2013
//! \brief	Texture configuration used by all texture nodes, which must be compatible
//!         between nodes to link them

#include "Pegasus/Texture/TextureConfiguration.h"

namespace Pegasus {
namespace Texture {


TextureConfiguration::TextureConfiguration(unsigned int width,
                                           unsigned int height,
                                           PixelFormat pixelFormat)
{
    mWidth = 1;
    SetWidth(width);

    mHeight = 1;
    SetHeight(height);

    mPixelFormat = PIXELFORMAT_RGB8;
    SetPixelFormat(pixelFormat);
}

//----------------------------------------------------------------------------------------

TextureConfiguration::TextureConfiguration(const TextureConfiguration & other)
{
    *this = other;
}

//----------------------------------------------------------------------------------------

TextureConfiguration & TextureConfiguration::operator=(const TextureConfiguration & other)
{
    PG_ASSERT(other.mWidth >= 1);
    PG_ASSERT(other.mHeight >= 1);
    PG_ASSERT(other.mPixelFormat < NUM_PIXELFORMATS);

    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mPixelFormat = other.mPixelFormat;

    return *this;
}

//----------------------------------------------------------------------------------------

void TextureConfiguration::SetWidth(unsigned int width)
{
    if (width >= 1)
    {
        mWidth = width;
    }
    else
    {
        PG_FAILSTR("Invalid width for a texture (%d), it must be >= 1", width);
    }
}

//----------------------------------------------------------------------------------------

void TextureConfiguration::SetHeight(unsigned int height)
{
    if (height >= 1)
    {
        mHeight = height;
    }
    else
    {
        PG_FAILSTR("Invalid height for a texture (%d), it must be >= 1", height);
    }
}

//----------------------------------------------------------------------------------------

void TextureConfiguration::SetPixelFormat(PixelFormat pixelFormat)
{
    if (pixelFormat < NUM_PIXELFORMATS)
    {
        mPixelFormat = pixelFormat;
    }
    else
    {
        PG_FAILSTR("Invalid pixel format for a texture (%d), it must be < %d", pixelFormat, NUM_PIXELFORMATS);
    }
}

//----------------------------------------------------------------------------------------

bool TextureConfiguration::IsCompatible(const TextureConfiguration & configuration) const
{
    return    (configuration.mWidth == mWidth)
           && (configuration.mHeight == mHeight)
           && (configuration.mPixelFormat == mPixelFormat);
}


}   // namespace Texture
}   // namespace Pegasus
