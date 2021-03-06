//! Add here format array. Order must match Pegasus::Core::Formats.h !//

const DXGI_FORMAT gFormatTranslations[Pegasus::Core::FORMAT_MAX_COUNT] =
{
       DXGI_FORMAT_R32G32B32A32_FLOAT,   // FORMAT_RGBA_32_FLOAT,
       DXGI_FORMAT_R32G32B32A32_UINT,    // FORMAT_RGBA_32_UINT,
       DXGI_FORMAT_R32G32B32A32_SINT,    // FORMAT_RGBA_32_SINT,
       DXGI_FORMAT_R32G32B32A32_TYPELESS,// FORMAT_RGBA_32_TYPELESS,
       DXGI_FORMAT_R32G32B32_FLOAT,      // FORMAT_RGB_32_FLOAT,
       DXGI_FORMAT_R32G32B32_UINT,       // FORMAT_RGB_32_UINT,
       DXGI_FORMAT_R32G32B32_SINT,       // FORMAT_RGB_32_SINT,
       DXGI_FORMAT_R32G32B32_TYPELESS,   // FORMAT_RGB_32_TYPELESS,
       DXGI_FORMAT_R32G32_FLOAT,         // FORMAT_RG_32_FLOAT,
       DXGI_FORMAT_R32G32_UINT,          //FORMAT_RG_32_UINT,
       DXGI_FORMAT_R32G32_SINT,          //FORMAT_RG_32_SINT,
       DXGI_FORMAT_R32G32_TYPELESS,      //FORMAT_RG_32_TYPELESS,
       DXGI_FORMAT_R16G16B16A16_FLOAT,   // FORMAT_RGBA_16_FLOAT,
       DXGI_FORMAT_R16G16B16A16_UINT,    // FORMAT_RGBA_16_UINT,
       DXGI_FORMAT_R16G16B16A16_SINT,    // FORMAT_RGBA_16_SINT,
       DXGI_FORMAT_R16G16B16A16_UNORM,   // FORMAT_RGBA_16_UNORM,
       DXGI_FORMAT_R16G16B16A16_SNORM,   // FORMAT_RGBA_16_SNORM,
       DXGI_FORMAT_R16G16B16A16_TYPELESS,// FORMAT_RGBA_16_TYPELESS,
       DXGI_FORMAT_R8G8B8A8_UINT,        // FORMAT_RGBA_8_UINT,
       DXGI_FORMAT_R8G8B8A8_SINT,        // FORMAT_RGBA_8_SINT,
       DXGI_FORMAT_R8G8B8A8_UNORM,       // FORMAT_RGBA_8_UNORM,
       DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,  // FORMAT_RGBA_8_UNORM_SRGB,
       DXGI_FORMAT_R8G8B8A8_SNORM,       // FORMAT_RGBA_8_SNORM,
       DXGI_FORMAT_R8G8B8A8_TYPELESS,    // FORMAT_RGBA_8_TYPELESS,
       DXGI_FORMAT_D32_FLOAT,            // FORMAT_D32_FLOAT,
       DXGI_FORMAT_R32_FLOAT,            // FORMAT_R32_FLOAT,
       DXGI_FORMAT_R32_UINT,             // FORMAT_R32_UINT,
       DXGI_FORMAT_R32_SINT,             // FORMAT_R32_SINT,
       DXGI_FORMAT_R32_TYPELESS,         // FORMAT_R32_TYPELESS,
       DXGI_FORMAT_D16_UNORM,            // FORMAT_D16_FLOAT,
       DXGI_FORMAT_R16_FLOAT,            // FORMAT_R16_FLOAT,
       DXGI_FORMAT_R16_UINT,             // FORMAT_R16_UINT,
       DXGI_FORMAT_R16_SINT,             // FORMAT_R16_SINT,
       DXGI_FORMAT_R16_UNORM,            // FORMAT_R16_UNORM,
       DXGI_FORMAT_R16_SNORM,            // FORMAT_R16_SNORM,
       DXGI_FORMAT_R16_TYPELESS,         // FORMAT_R16_TYPELESS,
       DXGI_FORMAT_R16G16_FLOAT,          // FORMAT_RG16_FLOAT,
       DXGI_FORMAT_R16G16_UINT,           // FORMAT_RG16_UINT,
       DXGI_FORMAT_R16G16_SINT,           // FORMAT_RG16_SINT,
       DXGI_FORMAT_R16G16_UNORM,          // FORMAT_RG16_UNORM,
       DXGI_FORMAT_R16G16_SNORM,          // FORMAT_RG16_SNORM,
       DXGI_FORMAT_R16G16_TYPELESS,       // FORMAT_RG16_TYPELESS,
       DXGI_FORMAT_R8_UNORM,             // FORMAT_R8_UNORM
       DXGI_FORMAT_R8_SINT,              // FORMAT_R8_SINT
       DXGI_FORMAT_R8_UINT,              // FORMAT_R8_UINT
       DXGI_FORMAT_R8_SNORM,             // FORMAT_R8_SNORM
       DXGI_FORMAT_R8_TYPELESS           // FORMAT_R8_TYPELESS
};

DXGI_FORMAT GetDxFormat(Pegasus::Core::Format format)
{
    PG_ASSERT(format >= 0 && format < Pegasus::Core::FORMAT_MAX_COUNT);
    return gFormatTranslations[static_cast<unsigned>(format)];
};
