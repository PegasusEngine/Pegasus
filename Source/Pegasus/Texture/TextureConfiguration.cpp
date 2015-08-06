/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureConfiguration.cpp
//! \author	Karolyn Boulanger
//! \date	02nd November 2013
//! \brief	Texture configuration used by all texture nodes, which must be compatible
//!         between nodes to link them

#include "Pegasus/Texture/TextureConfiguration.h"

namespace Pegasus {
namespace Texture {


TextureConfiguration::TextureConfiguration()
:   mType(TYPE_2D)
,   mPixelFormat(PIXELFORMAT_RGBA8)
,   mWidth(256)
,   mHeight(256)
,   mDepth(1)
,   mNumLayers(1)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
}

//----------------------------------------------------------------------------------------

TextureConfiguration::TextureConfiguration(Type type,
                                           PixelFormat pixelFormat,
                                           unsigned int width,
                                           unsigned int height,
                                           unsigned int depth,
                                           unsigned int numLayers)
#if PEGASUS_ENABLE_PROXIES
:   mProxy(this)
#endif
{
    // Copy the parameters with validation, and correct if required

    // Type
    if (type < NUM_TYPES)
    {
        mType = type;
    }
    else
    {
        PG_FAILSTR("Invalid type for a texture (%d), it must be < %d", type, NUM_TYPES);
        mType = TYPE_2D;
    }

    // Pixel format
    if (pixelFormat < NUM_PIXELFORMATS)
    {
        mPixelFormat = pixelFormat;
    }
    else
    {
        PG_FAILSTR("Invalid pixel format for a texture (%d), it must be < %d", pixelFormat, NUM_PIXELFORMATS);
        mPixelFormat = PIXELFORMAT_RGBA8;
    }

    // Width
    if (width >= 1)
    {
        mWidth = width;
    }
    else
    {
        PG_FAILSTR("Invalid width for a texture (%d), it must be >= 1", width);
        mWidth = 1;
    }

    // Height
    if ((mType == TYPE_1D) || (mType == TYPE_1D_ARRAY))
    {
        PG_ASSERTSTR(height == 1, "Invalid height for a 1D texture (%d), it must be == 1", height);
        mHeight = 1;
    }
    else
    {
        if (height >= 1)
        {
            mHeight = height;
        }
        else
        {
            PG_FAILSTR("Invalid height for a texture (%d), it must be >= 1", height);
            mHeight = 1;
        }
    }
    if ((mType == TYPE_CUBE) && (mWidth != mHeight))
    {
        PG_FAILSTR("Invalid height for a cube texture (%d), it must be equal to the width (%d)", mHeight, mWidth);
        mHeight = mWidth;
    }

    // Depth
    if (mType == TYPE_3D)
    {
        if (depth >= 1)
        {
            mDepth = depth;
        }
        else
        {
            PG_FAILSTR("Invalid depth for a texture (%d), it must be >= 1", depth);
            mDepth = 1;
        }
    }
    else
    {
        PG_ASSERTSTR(depth == 1, "Invalid depth for a non-3D texture (%d), it must be == 1", depth);
        mDepth = 1;
    }

    // Number of layers
    if ((mType == TYPE_1D_ARRAY) || (mType == TYPE_2D_ARRAY))
    {
        if (numLayers >= 1)
        {
            mNumLayers = numLayers;
        }
        else
        {
            PG_FAILSTR("Invalid number of layers for a texture array (%d), it must be >= 1", numLayers);
            mNumLayers = 1;
        }
    }
    else if (mType == TYPE_CUBE)
    {
        PG_ASSERTSTR(numLayers == 6, "Invalid number of layers for a cube texture (%d), it must be == 6", numLayers);
        mNumLayers = 6;
    }
    else
    {
        PG_ASSERTSTR(numLayers == 1, "Invalid number of layers for a non-array texture (%d), it must be == 1", numLayers);
        mNumLayers = 1;
    }
}

//----------------------------------------------------------------------------------------

TextureConfiguration::TextureConfiguration(const TextureConfiguration & other)
#if PEGASUS_ENABLE_PROXIES
:   mProxy(this)
#endif
{
    *this = other;
}

//----------------------------------------------------------------------------------------

TextureConfiguration & TextureConfiguration::operator=(const TextureConfiguration & other)
{
    PG_ASSERT(other.mType < NUM_TYPES);
    PG_ASSERT(other.mPixelFormat < NUM_PIXELFORMATS);
    PG_ASSERT(other.mWidth >= 1);
    PG_ASSERT(other.mHeight >= 1);
    PG_ASSERT(other.mDepth >= 1);
    PG_ASSERT(other.mNumLayers >= 1);

    mType = other.mType;
    mPixelFormat = other.mPixelFormat;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mDepth = other.mDepth;
    mNumLayers = other.mNumLayers;

    return *this;
}

//----------------------------------------------------------------------------------------

unsigned int TextureConfiguration::GetNumBytesPerPixel() const
{
    switch (mPixelFormat)
    {
        case PIXELFORMAT_RGBA8:         return 4;

        default:
            PG_FAILSTR("Invalid texture pixel format (%d), it should be less than %d", mPixelFormat, NUM_PIXELFORMATS);
            return 1;
    }
}

//----------------------------------------------------------------------------------------

bool TextureConfiguration::IsCompatible(const TextureConfiguration & configuration) const
{
    return    (configuration.mType == mType)
           && (configuration.mPixelFormat == mPixelFormat)
           && (configuration.mWidth == mWidth)
           && (configuration.mHeight == mHeight)
           && (configuration.mDepth == mDepth)
           && (configuration.mNumLayers == mNumLayers);
}


}   // namespace Texture
}   // namespace Pegasus
