/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXTextureFactory.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 texture factory implementation

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/Render.h"
#include "Pegasus/Render/TextureFactory.h"
#include "Pegasus/Texture/TextureData.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Memcpy.h"
#include "../Source/Pegasus/Render/DX11/DXGpuDataDefs.h"
#include "../Source/Pegasus/Render/DX11/DXRenderContext.h"
#include "Pegasus/Memory/MemoryManager.h"

using namespace Pegasus;
using namespace Pegasus::Render;

extern DXGI_FORMAT GetDxFormat(Pegasus::Core::Format format);

class DXTextureFactory : public Pegasus::Texture::ITextureFactory
{
public:
    DXTextureFactory();
    virtual ~DXTextureFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);

    virtual void GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData);

    virtual void DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData);

    void InternalCreateRenderTarget(const Pegasus::Render::RenderTargetConfig* config, const Pegasus::Render::CubeMap* cubeMap, const Pegasus::Render::VolumeTexture* volumeTexture, Pegasus::Render::CubeFace face, int volumeSlice, Pegasus::Render::RenderTarget& renderTarget);

    void InternalDestroyRenderTarget(Pegasus::Render::RenderTarget& renderTarget);

    void InternalCreateDepthStencil(const Pegasus::Render::DepthStencilConfig* config, Pegasus::Render::DepthStencil& depthStencil);
    
    void InternalDestroyDepthStencil(Pegasus::Render::DepthStencil& depthStencil);

    void InternalCreateCubeMap(const Pegasus::Render::CubeMapConfig& config, Pegasus::Render::CubeMap& cubeMap);

    void InternalCreateVolumeTexture(const Pegasus::Render::VolumeTextureConfig& config, Pegasus::Render::VolumeTexture& volTexture);

    void InternalDestroyCubeMap(Pegasus::Render::CubeMap& cubeMap);

    void InternalDestroyVolumeTexture(Pegasus::Render::VolumeTexture& volumeTexture);

private:

    Pegasus::Render::DXTextureGPUData * GetOrAllocateTextureGpuData(Pegasus::Texture::TextureData * nodeData); 
    void Get2DConfigTranslation(const Pegasus::Texture::TextureConfiguration& config, D3D11_TEXTURE2D_DESC& d3dDesc);
    bool ShouldRebuildTexture(const D3D11_TEXTURE2D_DESC& d3dDesc1, const D3D11_TEXTURE2D_DESC& d3dDesc2);
    Pegasus::Alloc::IAllocator* mAllocator;
};

DXTextureFactory::DXTextureFactory()
: mAllocator(nullptr)
{
}

Pegasus::Render::DXTextureGPUData * DXTextureFactory::GetOrAllocateTextureGpuData(Pegasus::Texture::TextureData* nodeData)
{
    Pegasus::Graph::NodeGPUData * nodeGpuData = nodeData->GetNodeGPUData();
    Pegasus::Render::DXTextureGPUData * texGpuData = nullptr;
    if (nodeGpuData == nullptr)
    {
        texGpuData = PG_NEW(
            mAllocator,
            -1,
            "DXTextureGPUData",
            Pegasus::Alloc::PG_MEM_TEMP
        )
        Pegasus::Render::DXTextureGPUData;
        Pegasus::Utils::Memset8(&texGpuData->mDesc, 0, sizeof(texGpuData->mDesc));
        Pegasus::Utils::Memset8(&texGpuData->mSrvDesc, 0, sizeof(texGpuData->mSrvDesc));
        Pegasus::Utils::Memset8(&texGpuData->mUavDesc, 0, sizeof(texGpuData->mUavDesc));
        nodeData->SetNodeGPUData(reinterpret_cast<Pegasus::Graph::NodeGPUData*>(texGpuData));
    }
    else
    {
        texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData, nodeGpuData);
    }
    return texGpuData;

}

void DXTextureFactory::Initialize(Pegasus::Alloc::IAllocator * allocator)
{
    mAllocator = allocator;
}

void DXTextureFactory::Get2DConfigTranslation(const Pegasus::Texture::TextureConfiguration& config, D3D11_TEXTURE2D_DESC& d3dDesc)
{
    PG_ASSERT(config.GetPixelFormat() < Core::FORMAT_MAX_COUNT);
    PG_ASSERTSTR(config.GetNumLayers() == 1, "Pegasus only supports 1 dimensional arrays for nowL");

    d3dDesc.Width = config.GetWidth();
    d3dDesc.Height = config.GetHeight();
    d3dDesc.MipLevels = 1;
    d3dDesc.ArraySize = config.GetNumLayers();
    d3dDesc.Format = GetDxFormat(config.GetPixelFormat());
    d3dDesc.SampleDesc.Count = 1;
    d3dDesc.SampleDesc.Quality = 0;
    d3dDesc.Usage = D3D11_USAGE_IMMUTABLE;
    d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    d3dDesc.CPUAccessFlags = 0;
    d3dDesc.MiscFlags = 0;
    
}

bool DXTextureFactory::ShouldRebuildTexture(const D3D11_TEXTURE2D_DESC& d3dDesc1, const D3D11_TEXTURE2D_DESC& d3dDesc2)
{
    return
    d3dDesc1.Width              != d3dDesc2.Width              &&   
    d3dDesc1.Height             != d3dDesc2.Height             &&
    d3dDesc1.MipLevels          != d3dDesc2.MipLevels          &&
    d3dDesc1.ArraySize          != d3dDesc2.ArraySize          &&
    d3dDesc1.Format             != d3dDesc2.Format;
}

void DXTextureFactory::GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXTextureGPUData* texGpuData = GetOrAllocateTextureGpuData(nodeData);
    const Pegasus::Texture::TextureConfiguration& config = nodeData->GetConfiguration();
    PG_ASSERTSTR(config.GetType() == Pegasus::Texture::TextureConfiguration::TYPE_2D, "Currently only support for 2d textures");

    D3D11_TEXTURE2D_DESC translation; 
    Get2DConfigTranslation(config, translation);
	bool doMap = true;

	if (texGpuData->mTexture == nullptr)	
	{
        PG_ASSERT(texGpuData->mSrv == nullptr);
		translation.Usage = D3D11_USAGE_IMMUTABLE;
		translation.CPUAccessFlags = 0;
	}
	else
	{
		translation.Usage = D3D11_USAGE_DYNAMIC;
		translation.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	
	}

    
    if (texGpuData->mDesc.Usage == D3D11_USAGE_IMMUTABLE || ShouldRebuildTexture(translation, texGpuData->mDesc))
    {
		doMap = false;
        texGpuData->mTexture = nullptr;
        texGpuData->mSrv = nullptr;

        D3D11_SUBRESOURCE_DATA srd; 
        srd.pSysMem = nodeData->GetLayerImageData(0);
        srd.SysMemPitch = translation.Width * config.GetNumBytesPerPixel();
        srd.SysMemSlicePitch = 0;        

        VALID_DECLARE(device->CreateTexture2D(&translation, &srd, &texGpuData->mTexture));
        
        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = texGpuData->mSrvDesc;
        srvDesc.Format = translation.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = translation.MipLevels;
        
        VALID(
            device->CreateShaderResourceView(
                texGpuData->mTexture,
                &texGpuData->mSrvDesc,
                &texGpuData->mSrv
            )
        );
    }

    if (doMap)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (context->Map(texGpuData->mTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
        {
            PG_ASSERTSTR(mappedResource.pData != nullptr,"map returned a null pointer of data!");
            Pegasus::Utils::Memcpy(mappedResource.pData, nodeData->GetLayerImageData(0), config.GetNumBytesPerLayer());
            context->Unmap(texGpuData->mTexture, 0);
        }
        else
        {
            PG_FAILSTR("GPU Map failed!");
        }
    }

    texGpuData->mDesc = translation;
    nodeData->ValidateGPUData();
}

void DXTextureFactory::DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData)
{
    Pegasus::Graph::NodeGPUData * nodeGpuData = nodeData->GetNodeGPUData(); 
    if (nodeGpuData != nullptr)
    {
        Pegasus::Render::DXTextureGPUData* texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData, nodeGpuData);
        texGpuData->mTexture = nullptr;
        texGpuData->mSrv = nullptr;
        PG_DELETE(mAllocator, texGpuData);
        nodeData->SetNodeGPUData(nullptr);
    }
   
}

void DXTextureFactory::InternalCreateRenderTarget(
    const Pegasus::Render::RenderTargetConfig* config, 
    const Pegasus::Render::CubeMap* cubeMap,
    const Pegasus::Render::VolumeTexture* volumeTexture,
    Pegasus::Render::CubeFace face,
    int volumeSlice,
    Pegasus::Render::RenderTarget& renderTarget
)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXRenderTargetGPUData* renderTargetGpuData = PG_NEW (
        mAllocator,
        -1,
        "DXRenderTargetGPUData",
        Pegasus::Alloc::PG_MEM_PERM
    ) Pegasus::Render::DXRenderTargetGPUData; 
    Utils::Memset8(&renderTargetGpuData->mTextureView, 0, sizeof(renderTargetGpuData->mTextureView));
    Utils::Memset8(&renderTargetGpuData->mTextureView3d, 0, sizeof(renderTargetGpuData->mTextureView3d));

    Pegasus::Render::RenderTargetConfig outputConfig;

    D3D11_TEXTURE2D_DESC& texDesc = renderTargetGpuData->mTextureView.mDesc;
    if (config != nullptr)
    {
        renderTargetGpuData->mDim = DXRenderTargetGPUData::Dim_2d;
        texDesc.Width = static_cast<unsigned int>(config->mWidth);
        texDesc.Height = static_cast<unsigned int>(config->mHeight);
        texDesc.MipLevels = static_cast<unsigned int>(config->mMipCount);
        texDesc.ArraySize = 1;
        texDesc.Format = GetDxFormat(config->mFormat);
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = (texDesc.MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

        VALID_DECLARE(device->CreateTexture2D(&texDesc, nullptr, &renderTargetGpuData->mTextureView.mTexture));
        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = renderTargetGpuData->mTextureView.mSrvDesc;
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    
        VALID(device->CreateShaderResourceView(renderTargetGpuData->mTextureView.mTexture, &srvDesc, &renderTargetGpuData->mTextureView.mSrv));

        D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc = renderTargetGpuData->mTextureView.mUavDesc;
        uavDesc.Format = srvDesc.Format;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;

        outputConfig = *config;
        VALID(device->CreateUnorderedAccessView(renderTargetGpuData->mTextureView.mTexture, &uavDesc, &renderTargetGpuData->mTextureView.mUav));

    }
    else if (cubeMap != nullptr)
    {   
        renderTargetGpuData->mDim = DXRenderTargetGPUData::Dim_2d;
        const Pegasus::Render::DXTextureGPUData* texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData, cubeMap->GetInternalData());
        renderTargetGpuData->mTextureView.mDesc = texGpuData->mDesc;
        renderTargetGpuData->mTextureView.mTexture = texGpuData->mTexture;
        renderTargetGpuData->mTextureView.mSrvDesc = texGpuData->mSrvDesc;
        renderTargetGpuData->mTextureView.mSrv = texGpuData->mSrv;
        Utils::Memset8(&renderTargetGpuData->mTextureView.mUavDesc, 0 , sizeof(renderTargetGpuData->mTextureView.mUavDesc));

        const Pegasus::Render::CubeMapConfig& cubeConfig = cubeMap->GetConfig();
        outputConfig.mWidth  = cubeConfig.mWidth;
        outputConfig.mHeight = cubeConfig.mHeight;
        outputConfig.mFormat = cubeConfig.mFormat;
    }
    else
    {
        renderTargetGpuData->mDim = DXRenderTargetGPUData::Dim_3d;
        PG_ASSERT(volumeTexture != nullptr);        
        const Pegasus::Render::DXTextureGPUData3d* texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData3d, volumeTexture->GetInternalData());
        renderTargetGpuData->mTextureView3d.mDesc = texGpuData->mDesc;
        renderTargetGpuData->mTextureView3d.mTexture = texGpuData->mTexture;
        renderTargetGpuData->mTextureView3d.mSrvDesc = texGpuData->mSrvDesc;
        renderTargetGpuData->mTextureView3d.mSrv = texGpuData->mSrv;
        const Pegasus::Render::VolumeTextureConfig& volumeConfig = volumeTexture->GetConfig();
        outputConfig.mWidth  = volumeConfig.mWidth;
        outputConfig.mHeight = volumeConfig.mHeight;
        outputConfig.mFormat = volumeConfig.mFormat;
    }

    D3D11_RENDER_TARGET_VIEW_DESC& rtDesc = renderTargetGpuData->mDesc;
    rtDesc.Format = renderTargetGpuData->mDim == DXRenderTargetGPUData::Dim_2d ?renderTargetGpuData->mTextureView.mDesc.Format :renderTargetGpuData->mTextureView3d.mDesc.Format  ;
    ID3D11Resource* resource = nullptr;
    if (cubeMap != nullptr)
    {
        static const D3D11_TEXTURECUBE_FACE gFaceTranslation[] = {
            D3D11_TEXTURECUBE_FACE_POSITIVE_X,
            D3D11_TEXTURECUBE_FACE_NEGATIVE_X,
            D3D11_TEXTURECUBE_FACE_POSITIVE_Y,
            D3D11_TEXTURECUBE_FACE_NEGATIVE_Y,
            D3D11_TEXTURECUBE_FACE_POSITIVE_Z,
            D3D11_TEXTURECUBE_FACE_NEGATIVE_Z
        };

        rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtDesc.Texture2DArray.MipSlice = 0;
        rtDesc.Texture2DArray.FirstArraySlice = gFaceTranslation[face];
        rtDesc.Texture2DArray.ArraySize = 1; 
        resource = renderTargetGpuData->mTextureView.mTexture;
    }
    else if (volumeTexture != nullptr)
    {
        rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        rtDesc.Texture3D.MipSlice = 0;
        rtDesc.Texture3D.FirstWSlice = (unsigned)volumeSlice;
        rtDesc.Texture3D.WSize = 1;        
        resource = renderTargetGpuData->mTextureView3d.mTexture;
    }
    else
    {
        resource = renderTargetGpuData->mTextureView.mTexture;
        rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtDesc.Texture2D.MipSlice = static_cast<unsigned int>(outputConfig.mMipStart);
    }

    VALID_DECLARE(device->CreateRenderTargetView(resource, &rtDesc, &renderTargetGpuData->mRenderTarget));

    renderTarget.SetConfig(outputConfig);
    renderTarget.SetInternalData(static_cast<void*>(renderTargetGpuData));
}

void DXTextureFactory::InternalDestroyRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
    PG_ASSERT(renderTarget.GetInternalData() != nullptr);
    Pegasus::Render::DXRenderTargetGPUData * renderTargetGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget.GetInternalData());
    renderTargetGpuData->mRenderTarget = nullptr;
    renderTargetGpuData->mTextureView.mSrv = nullptr;
    renderTargetGpuData->mTextureView.mTexture = nullptr;
    PG_DELETE(mAllocator, renderTargetGpuData);
    renderTarget.SetInternalData(nullptr);
}

void DXTextureFactory::InternalCreateDepthStencil(const Pegasus::Render::DepthStencilConfig* config, Pegasus::Render::DepthStencil& depthStencil)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXDepthStencilGPUData* depthStencilGpuData = PG_NEW (
        mAllocator,
        -1,
        "DXDepthStencilGPUData",
        Pegasus::Alloc::PG_MEM_PERM
    ) Pegasus::Render::DXDepthStencilGPUData; 

    depthStencilGpuData->mUseStencil = config->mUseStencil > 0;

    Utils::Memset8(&depthStencilGpuData->mTextureDesc, 0, sizeof(depthStencilGpuData->mTextureDesc));
    Utils::Memset8(&depthStencilGpuData->mSrvDepthDesc, 0, sizeof(depthStencilGpuData->mSrvDepthDesc));
    Utils::Memset8(&depthStencilGpuData->mSrvStencilDesc, 0, sizeof(depthStencilGpuData->mSrvStencilDesc));
    Utils::Memset8(&depthStencilGpuData->mDepthViewDesc, 0, sizeof(depthStencilGpuData->mDepthViewDesc));

    //Create core texture with the depth definitions.
    D3D11_TEXTURE2D_DESC& texDesc = depthStencilGpuData->mTextureDesc;
    texDesc.Width = static_cast<unsigned int>(config->mWidth);
    texDesc.Height = static_cast<unsigned int>(config->mHeight);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = depthStencilGpuData->mUseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    VALID_DECLARE(device->CreateTexture2D(&texDesc, nullptr, &depthStencilGpuData->mTexture));


    // Create SRV for depth
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC& srvDepthDesc = depthStencilGpuData->mSrvDepthDesc;
        srvDepthDesc.Format = depthStencilGpuData->mUseStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
        srvDepthDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDepthDesc.Texture2D.MostDetailedMip = 0;
        srvDepthDesc.Texture2D.MipLevels = texDesc.MipLevels;
        VALID(device->CreateShaderResourceView(depthStencilGpuData->mTexture, &srvDepthDesc, &depthStencilGpuData->mSrvDepth));
    }

    // Create SRV for stencil if stencil is enabled
    if (depthStencilGpuData->mUseStencil)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC& srvStencilDesc = depthStencilGpuData->mSrvDepthDesc;
        srvStencilDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
        srvStencilDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvStencilDesc.Texture2D.MostDetailedMip = 0;
        srvStencilDesc.Texture2D.MipLevels = texDesc.MipLevels;

        VALID(device->CreateShaderResourceView(depthStencilGpuData->mTexture, &srvStencilDesc, &depthStencilGpuData->mSrvStencil));
    }

    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC& depthViewDesc = depthStencilGpuData->mDepthViewDesc;
    depthViewDesc.Format = depthStencilGpuData->mUseStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Flags = 0;
    depthViewDesc.Texture2D.MipSlice = 0;

    VALID(device->CreateDepthStencilView(depthStencilGpuData->mTexture, &depthViewDesc, &depthStencilGpuData->mDepthView));

    depthStencil.SetConfig(*config);
    depthStencil.SetInternalData(static_cast<void*>(depthStencilGpuData));
}

void DXTextureFactory::InternalDestroyDepthStencil(Pegasus::Render::DepthStencil& depthStencil)
{
    PG_ASSERT(depthStencil.GetInternalData() != nullptr);
    Pegasus::Render::DXDepthStencilGPUData * depthStencilGPUData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXDepthStencilGPUData, depthStencil.GetInternalData());
    depthStencilGPUData->mDepthView = nullptr;
    depthStencilGPUData->mSrvDepth = nullptr;
    depthStencilGPUData->mSrvStencil = nullptr;
    depthStencilGPUData->mTexture = nullptr;
    
    PG_DELETE(mAllocator, depthStencilGPUData);
    depthStencil.SetInternalData(nullptr);
}

void DXTextureFactory::InternalCreateCubeMap(const Pegasus::Render::CubeMapConfig& config, Pegasus::Render::CubeMap& cubeMap)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXTextureGPUData* texGpuData  = PG_NEW (
        mAllocator,
        -1,
        "DX Texture Cube",
        Pegasus::Alloc::PG_MEM_PERM
    ) Pegasus::Render::DXTextureGPUData;

    D3D11_TEXTURE2D_DESC& texDesc = texGpuData->mDesc;
    texDesc.Width = static_cast<unsigned int>(config.mWidth);
    texDesc.Height = static_cast<unsigned int>(config.mHeight);
    texDesc.MipLevels = config.mMipCount;
    texDesc.ArraySize = 6;
    texDesc.Format = GetDxFormat(config.mFormat);
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | (texDesc.MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

    VALID_DECLARE(device->CreateTexture2D(&texDesc, nullptr, &texGpuData->mTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = texGpuData->mSrvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
    
    VALID(device->CreateShaderResourceView(texGpuData->mTexture, &srvDesc, &texGpuData->mSrv));
    
    cubeMap.SetConfig(config);
    cubeMap.SetInternalData(texGpuData);   

    D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc = texGpuData->mUavDesc;
    uavDesc.Format = texDesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.Texture2DArray.MipSlice = 0;
    uavDesc.Texture2DArray.FirstArraySlice = 0;
    uavDesc.Texture2DArray.ArraySize = 6;

    VALID(device->CreateUnorderedAccessView(texGpuData->mTexture, &uavDesc, &texGpuData->mUav));

}

void DXTextureFactory::InternalCreateVolumeTexture(const Pegasus::Render::VolumeTextureConfig& config, Pegasus::Render::VolumeTexture& volTex)
{
    ID3D11DeviceContext * context;
    ID3D11Device * device;
    Pegasus::Render::GetDeviceAndContext(&device, &context);

    Pegasus::Render::DXTextureGPUData3d* texGpuData  = PG_NEW (
        mAllocator,
        -1,
        "InternalTexData",
        Pegasus::Alloc::PG_MEM_PERM
    ) Pegasus::Render::DXTextureGPUData3d;

    D3D11_TEXTURE3D_DESC& texDesc = texGpuData->mDesc;
    texDesc.Width = static_cast<unsigned int>(config.mWidth);
    texDesc.Height = static_cast<unsigned int>(config.mHeight);
    texDesc.Depth = static_cast<unsigned int>(config.mDepth);
    texDesc.MipLevels = 1;
    texDesc.Format = GetDxFormat(config.mFormat);
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    VALID_DECLARE(device->CreateTexture3D(&texDesc, nullptr, &texGpuData->mTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = texGpuData->mSrvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    srvDesc.Texture3D.MostDetailedMip = 0;
    srvDesc.Texture3D.MipLevels = texDesc.MipLevels;
    
    VALID(device->CreateShaderResourceView(texGpuData->mTexture, &srvDesc, &texGpuData->mSrv));
    
    volTex.SetConfig(config);
    volTex.SetInternalData(texGpuData);   

    D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc = texGpuData->mUavDesc;
    uavDesc.Format = texDesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    uavDesc.Texture3D.MipSlice = 0;
    uavDesc.Texture3D.FirstWSlice = 0;
    uavDesc.Texture3D.WSize = static_cast<unsigned int>(config.mDepth);

    VALID(device->CreateUnorderedAccessView(texGpuData->mTexture, &uavDesc, &texGpuData->mUav));
}

void DXTextureFactory::InternalDestroyCubeMap(Pegasus::Render::CubeMap& cubeMap)
{
    Pegasus::Render::DXTextureGPUData* texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData, cubeMap.GetInternalData());
    texGpuData->mTexture = nullptr;
    texGpuData->mSrv = nullptr;
    PG_DELETE(mAllocator, texGpuData);
}

void DXTextureFactory::InternalDestroyVolumeTexture(Pegasus::Render::VolumeTexture& volumeTexture)
{
    Pegasus::Render::DXTextureGPUData3d* texGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXTextureGPUData3d, volumeTexture.GetInternalData());
    texGpuData->mTexture = nullptr;
    texGpuData->mSrv = nullptr;
    PG_DELETE(mAllocator, texGpuData);
}

namespace Pegasus
{

namespace Render
{
DXTextureFactory gTextureFactory;
Texture::ITextureFactory * GetRenderTextureFactory()
{
    return &gTextureFactory;
}

}

}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATE RENDER TARGET IMPLEMENTATION /////////////////////////// 
///////////////////////////////////////////////////////////////////////////////
Pegasus::Render::RenderTargetRef Pegasus::Render::CreateRenderTarget(Pegasus::Render::RenderTargetConfig& config)
{
    RenderTarget* rt = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "RenderTarget", Pegasus::Alloc::PG_MEM_PERM) RenderTarget(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateRenderTarget(&config, nullptr, nullptr, Pegasus::Render::X /*unused*/, 0/*unused*/, *rt);    
    return rt;
}

Pegasus::Render::DepthStencilRef Pegasus::Render::CreateDepthStencil(const Pegasus::Render::DepthStencilConfig& config)
{
    DepthStencil* ds = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "DepthStencil", Pegasus::Alloc::PG_MEM_PERM) DepthStencil(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateDepthStencil(&config, *ds);    
    return ds;
}

Pegasus::Render::RenderTargetRef Pegasus::Render::CreateRenderTargetFromCubeMap(Pegasus::Render::CubeFace targetFace, Pegasus::Render::CubeMapRef& cubeMap)
{
    RenderTarget* rt = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "RenderTarget", Pegasus::Alloc::PG_MEM_PERM) RenderTarget(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateRenderTarget(nullptr/*unused*/, &(*cubeMap), nullptr, /*unused*/ targetFace, 0/*unused*/,  *rt);
    return rt;
}

Pegasus::Render::RenderTargetRef Pegasus::Render::CreateRenderTargetFromVolumeTexture(int sliceIndex, Pegasus::Render::VolumeTextureRef& volTextureRef)
{
    RenderTarget* rt = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "RenderTarget", Pegasus::Alloc::PG_MEM_PERM) RenderTarget(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateRenderTarget(nullptr/*unused*/, nullptr/*unused*/, &(*volTextureRef), /*unused*/ Pegasus::Render::X, sliceIndex/*unused*/,  *rt);
    return rt;
}

Pegasus::Render::CubeMapRef Pegasus::Render::CreateCubeMap(const Pegasus::Render::CubeMapConfig& config)
{
    CubeMap* cubeMap = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "RenderTarget", Pegasus::Alloc::PG_MEM_PERM) CubeMap(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateCubeMap(config, *cubeMap);
    return cubeMap;
}

Pegasus::Render::VolumeTextureRef Pegasus::Render::CreateVolumeTexture(const Pegasus::Render::VolumeTextureConfig& config)
{
    VolumeTexture* volTex = PG_NEW(Pegasus::Memory::GetRenderAllocator(), -1, "RenderTarget", Pegasus::Alloc::PG_MEM_PERM) VolumeTexture(Pegasus::Memory::GetRenderAllocator());
    Pegasus::Render::gTextureFactory.InternalCreateVolumeTexture(config, *volTex);
    return volTex;
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE RENDER TARGET IMPLEMENTATION       /////////////////////
///////////////////////////////////////////////////////////////////////////////

template<>
Pegasus::Render::BasicResource<Pegasus::Render::RenderTargetConfig>::~BasicResource()
{
    Pegasus::Render::gTextureFactory.InternalDestroyRenderTarget(*this);
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::DepthStencilConfig>::~BasicResource()
{
    Pegasus::Render::gTextureFactory.InternalDestroyDepthStencil(*this);
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::CubeMapConfig>::~BasicResource()
{
    Pegasus::Render::gTextureFactory.InternalDestroyCubeMap(*this);
}

template<>
Pegasus::Render::BasicResource<Pegasus::Render::VolumeTextureConfig>::~BasicResource()
{
    Pegasus::Render::gTextureFactory.InternalDestroyVolumeTexture(*this);
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
