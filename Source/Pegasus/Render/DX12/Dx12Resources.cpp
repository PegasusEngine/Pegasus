/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Resources.cpp
//! \author Kleber Garcia
//! \date   September 30th 2018
//! \brief  Gpu Resources. And their views.

#include "Dx12Resources.h"
#include "Dx12MemMgr.h"
#include "Dx12Defs.h"

namespace Pegasus
{
namespace Render
{

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

Dx12Resource::Dx12Resource(const ResourceDesc& desc, Dx12Device* device)
: mDesc(desc), mDevice(device), Core::RefCounted(device->GetAllocator())
{
    {
        mData.viewDesc = {};
        mData.resDesc = {};
        mData.resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		mData.heapFlags = D3D12_HEAP_FLAG_NONE;
        mData.resource = nullptr;

        //Core resource setup
        if (!!(mDesc.bindFlags & BindFlags_Srv)
        ||  !!(mDesc.bindFlags & BindFlags_Uav))
        {
            mViewData.handle = mDevice->GetMemMgr()->AllocateSrvOrUavOrCbv();
            if (!!(mDesc.bindFlags & BindFlags_Uav))
                mData.resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        else
        {
                mData.resDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }

        if (!!(mDesc.bindFlags & BindFlags_Rt) || !!(mDesc.bindFlags & BindFlags_Ds))
        {
            if (!!(mDesc.bindFlags & BindFlags_Rt))
                mData.resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            if (!!(mDesc.bindFlags & BindFlags_Ds))
                mData.resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }
    }

    PG_ASSERTSTR(desc.usage != ResourceUsage_Dynamic, "Dynamic resources not supported yet.");
    mData.heapProps.Type = desc.usage == ResourceUsage_Static ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_READBACK;
    mData.heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    mData.heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    mData.heapProps.CreationNodeMask = 0u;
    mData.heapProps.VisibleNodeMask = 0u;

}

Dx12Resource::~Dx12Resource()
{
    if (!!(mDesc.bindFlags & BindFlags_Srv)
    ||  !!(mDesc.bindFlags & BindFlags_Uav))
    {
        mDevice->GetMemMgr()->Delete(mViewData.handle);
    }

    if (mData.resource != nullptr)
    {
		mData.resource->Release();
        mData.resource = nullptr;
    }
}

void Dx12Resource::init()
{
    DX_VALID_DECLARE(mDevice->GetD3D()->CreateCommittedResource(
        &mData.heapProps, mData.heapFlags, &mData.resDesc,
        D3D12_RESOURCE_STATE_COMMON, nullptr,
        __uuidof(ID3D12Resource),
        reinterpret_cast<void**>(&mData.resource)
    ));

    if (!!(mDesc.bindFlags & BindFlags_Srv))
    {
		mDevice->GetD3D()->CreateShaderResourceView(
			mData.resource, &mData.viewDesc, mViewData.handle.handle);
    }
}

Dx12Texture::Dx12Texture(const TextureDesc& desc, Dx12Device* device)
: mDesc(desc), Dx12Resource(desc, device)
{
    D3D12_RESOURCE_DIMENSION dim = D3D12_RESOURCE_DIMENSION_UNKNOWN;
    if (mDesc.type == TextureType_1d)
        dim = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    else if (mDesc.type == TextureType_2d)
        dim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    else if (mDesc.type == TextureType_3d)
        dim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    PG_ASSERTSTR(mDesc.type != TextureType_CubeMap, "Cube map not yet supported.");

    auto dxFormat = GetDxFormat(desc.format);
    if (!!(mDesc.bindFlags & BindFlags_Srv))
    {
        auto& srvDesc = mData.viewDesc;
        srvDesc.Format = dxFormat;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        if (mDesc.type == TextureType_1d)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MostDetailedMip = 0u;
            srvDesc.Texture1D.MipLevels = desc.mipLevels;
            srvDesc.Texture1D.ResourceMinLODClamp = 0u;
        }
        else if (mDesc.type == TextureType_2d)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0u;
            srvDesc.Texture2D.MipLevels = desc.mipLevels;
            srvDesc.Texture2D.ResourceMinLODClamp = 0u;
            srvDesc.Texture2D.PlaneSlice = 0u;
        }
        else if (mDesc.type == TextureType_3d)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip = 0u;
            srvDesc.Texture3D.MipLevels = desc.mipLevels;
            srvDesc.Texture3D.ResourceMinLODClamp = 0u;
        }
    }

    mData.heapFlags |= D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
    mData.resDesc.Format = dxFormat;
    mData.resDesc.Dimension = dim;
    mData.resDesc.Alignment = 0u;
    mData.resDesc.Width = desc.width;
    mData.resDesc.Height = desc.height;
    mData.resDesc.DepthOrArraySize = desc.depth;
    mData.resDesc.MipLevels = desc.mipLevels;
    mData.resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    //mData.resDesc.Flags = check Dx12Resource constructor;
    mData.resDesc.SampleDesc.Count = 1u;
    mData.resDesc.SampleDesc.Quality = 0u;
}

Dx12Texture::~Dx12Texture()
{
}

void Dx12Texture::init()
{
    Dx12Resource::init();
}

Dx12Buffer::Dx12Buffer(const BufferDesc& desc, Dx12Device* device)
: mDesc(desc), Dx12Resource(desc, device)
{
    if (!!(mDesc.bindFlags & BindFlags_Srv))
    {
        auto& viewDesc = mData.viewDesc;
        viewDesc.Format = desc.bufferType == BufferType_Raw ? DXGI_FORMAT_R32_TYPELESS : desc.bufferType == BufferType_Structured ? DXGI_FORMAT_UNKNOWN : GetDxFormat(desc.format);
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        viewDesc.Buffer.FirstElement = 0u;
        viewDesc.Buffer.NumElements = desc.elementCount;
        viewDesc.Buffer.StructureByteStride = desc.bufferType == BufferType_Structured ? desc.stride : 0;
        viewDesc.Buffer.Flags = desc.bufferType == BufferType_Raw ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
    }

    mData.resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    mData.resDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    mData.resDesc.Width = desc.stride*desc.elementCount;
    mData.resDesc.Height = 1u;
    mData.resDesc.DepthOrArraySize = 1u;
    mData.resDesc.MipLevels = 1u;
    mData.resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //mData.resDesc.Flags = check Dx12Resource constructor;
    mData.resDesc.Format = DXGI_FORMAT_UNKNOWN;
    mData.resDesc.SampleDesc.Count = 1u;
    mData.resDesc.SampleDesc.Quality = 0u;
}

Dx12Buffer::~Dx12Buffer()
{
}

void Dx12Buffer::init()
{
    Dx12Resource::init();
}

}
}
