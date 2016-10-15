/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Formats.h
//! \author Kleber Garcia
//! \date   July 10, 2016
//! \brief  file containing format mappings across internal graphics API.

#ifndef PEGASUS_CORE_FORMATS_H
#define PEGASUS_CORE_FORMATS_H

namespace Pegasus
{
namespace Core
{

    //! Format list supported in pegasus.
    //! \WARNING: some of these formats might not be available depending on the platform used.
    enum Format
    {
        FORMAT_RGBA_32_FLOAT,
        FORMAT_RGBA_32_UINT,
        FORMAT_RGBA_32_SINT,
        FORMAT_RGBA_32_TYPELESS,
        FORMAT_RGB_32_FLOAT,
        FORMAT_RGB_32_UINT,
        FORMAT_RGB_32_SINT,
        FORMAT_RGB_32_TYPELESS,
        FORMAT_RG_32_FLOAT,
        FORMAT_RG_32_UINT,
        FORMAT_RG_32_SINT,
        FORMAT_RG_32_TYPELESS,
        FORMAT_RGBA_16_FLOAT,
        FORMAT_RGBA_16_UINT,
        FORMAT_RGBA_16_SINT,
        FORMAT_RGBA_16_UNORM,
        FORMAT_RGBA_16_SNORM,
        FORMAT_RGBA_16_TYPELESS,
        FORMAT_RGBA_8_UINT,
        FORMAT_RGBA_8_SINT,
        FORMAT_RGBA_8_UNORM,
        FORMAT_RGBA_8_UNORM_SRGB,
        FORMAT_RGBA_8_SNORM,
        FORMAT_RGBA_8_TYPELESS,
        FORMAT_D32_FLOAT,
        FORMAT_R32_FLOAT,
        FORMAT_R32_UINT,
        FORMAT_R32_SINT,
        FORMAT_R32_TYPELESS,
        FORMAT_D16_UNORM,
        FORMAT_R16_FLOAT,
        FORMAT_R16_UINT,
        FORMAT_R16_SINT,
        FORMAT_R16_UNORM,
        FORMAT_R16_SNORM,
        FORMAT_R16_TYPELESS,
        FORMAT_RG16_FLOAT,
        FORMAT_RG16_UINT,
        FORMAT_RG16_SINT,
        FORMAT_RG16_UNORM,
        FORMAT_RG16_SNORM,
        FORMAT_RG16_TYPELESS,
        FORMAT_R8_UNORM,
        FORMAT_R8_SINT,
        FORMAT_R8_UINT,
        FORMAT_R8_SNORM,
        FORMAT_R8_TYPELESS,
        FORMAT_MAX_COUNT
    };
}
}

#endif
