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

class DXTextureFactory : public Pegasus::Texture::ITextureFactory
{
public:
    DXTextureFactory();
    virtual ~DXTextureFactory(){}

    virtual void Initialize(Pegasus::Alloc::IAllocator * allocator);

    virtual void GenerateTextureGPUData(Pegasus::Texture::TextureData * nodeData);

    virtual void DestroyNodeGPUData(Pegasus::Texture::TextureData * nodeData);

    void InternalCreateRenderTarget(const Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& renderTarget);

    void InternalDestroyRenderTarget(Pegasus::Render::RenderTarget& renderTarget);

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
    PG_ASSERT(config.GetPixelFormat() < Pegasus::Texture::TextureConfiguration::NUM_PIXELFORMATS);
    PG_ASSERTSTR(config.GetNumLayers() == 1, "Pegasus only supports 1 dimensional arrays for nowL");

    static const DXGI_FORMAT sTextureFormatTranslation[Pegasus::Texture::TextureConfiguration::NUM_PIXELFORMATS] = {
        DXGI_FORMAT_R8G8B8A8_UNORM
    };
    d3dDesc.Width = config.GetWidth();
    d3dDesc.Height = config.GetHeight();
    d3dDesc.MipLevels = 1;
    d3dDesc.ArraySize = config.GetNumLayers();
    d3dDesc.Format = sTextureFormatTranslation[config.GetPixelFormat()];
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
        PG_DELETE(mAllocator, texGpuData);
        nodeData->SetNodeGPUData(nullptr);
    }
   
}

void DXTextureFactory::InternalCreateRenderTarget(const Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& renderTarget)
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

    D3D11_TEXTURE2D_DESC& texDesc = renderTargetGpuData->mTextureView.mDesc;
    texDesc.Width = static_cast<unsigned int>(config.mWidth);
    texDesc.Height = static_cast<unsigned int>(config.mHeight);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    VALID_DECLARE(device->CreateTexture2D(&texDesc, nullptr, &renderTargetGpuData->mTextureView.mTexture));

    D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc = renderTargetGpuData->mTextureView.mSrvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    
    VALID(device->CreateShaderResourceView(renderTargetGpuData->mTextureView.mTexture, &srvDesc, &renderTargetGpuData->mTextureView.mSrv));

    D3D11_RENDER_TARGET_VIEW_DESC& rtDesc = renderTargetGpuData->mDesc;
    rtDesc.Format = texDesc.Format;
    rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtDesc.Texture2D.MipSlice = 0;

    VALID(device->CreateRenderTargetView(renderTargetGpuData->mTextureView.mTexture, &rtDesc, &renderTargetGpuData->mRenderTarget));

    renderTarget.mConfig = config;
    renderTarget.mInternalData = static_cast<void*>(renderTargetGpuData);
}

void DXTextureFactory::InternalDestroyRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
    PG_ASSERT(renderTarget.mInternalData != nullptr);
    Pegasus::Render::DXRenderTargetGPUData * renderTargetGpuData = PEGASUS_GRAPH_GPUDATA_SAFECAST(Pegasus::Render::DXRenderTargetGPUData, renderTarget.mInternalData);
    renderTargetGpuData->mRenderTarget = nullptr;
    renderTargetGpuData->mTextureView.mSrv = nullptr;
    renderTargetGpuData->mTextureView.mTexture = nullptr;
    PG_DELETE(mAllocator, renderTargetGpuData);
    renderTarget.mInternalData = nullptr;
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
void Pegasus::Render::CreateRenderTarget(Pegasus::Render::RenderTargetConfig& config, Pegasus::Render::RenderTarget& renderTarget)
{
    Pegasus::Render::gTextureFactory.InternalCreateRenderTarget(config, renderTarget);    
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRenderTarget(Pegasus::Render::RenderTarget& renderTarget)
{
    Pegasus::Render::gTextureFactory.InternalDestroyRenderTarget(renderTarget);
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
