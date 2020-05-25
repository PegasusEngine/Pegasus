/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Resources.h
//! \author Kleber Garcia
//! \date   September 30th 2018
//! \brief  Gpu Resources. And their views.

#pragma once

#include <Pegasus/Render/Render.h>
#include <Pegasus/Core/Formats.h>
#include <Pegasus/Core/RefCounted.h>
#include <Pegasus/Core/Ref.h>
#include "Dx12Defs.h"
#include "Dx12Device.h"
#include "Dx12RDMgr.h"
#include <vector>
#include <d3d12.h>
#include <atlbase.h>
#include <string>

namespace Pegasus
{
namespace Render
{

class Dx12Device;
class Dx12Resource;
struct GpuMemoryBlock;

DXGI_FORMAT GetDxFormat(Pegasus::Core::Format format);

class Dx12Resource
{
    friend class Dx12ResourceTable;
public:
    Dx12Resource(const ResourceConfig& desc, Dx12Device* device);
    virtual ~Dx12Resource();
    ID3D12Resource* GetD3D() const { return mData.resource; }
    ID3D12Resource* GetD3D() { return mData.resource; }
    virtual void init();

    static Dx12Resource* GetDx12Resource(IResource* res);
    static const Dx12Resource* GetDx12Resource(const IResource* res);

    const ResourceConfig& GetResConfig() const { return mResConfig; }
    
    D3D12_RESOURCE_STATES GetDefaultState() const { return mDefaultResourceState; }
    D3D12_GPU_VIRTUAL_ADDRESS GetVA() const { return mData.gpuVirtualAddress; }
    void* GetDx12ResourceGpuPtr();

protected:
    Dx12RDMgr::Handle mSrvHandle;
    Dx12RDMgr::Handle mUavHandle;

    struct ResourceData
    {
        D3D12_HEAP_PROPERTIES heapProps;
        D3D12_RESOURCE_DESC resDesc;
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        D3D12_HEAP_FLAGS heapFlags;
        ID3D12Resource* resource;
        void* mappedMemory;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
    } mData;

    Dx12Device* mDevice;
    D3D12_RESOURCE_STATES mDefaultResourceState;

private:
    ResourceConfig mResConfig;
};

class Dx12Texture : public Pegasus::Render::Texture, public Dx12Resource
{
public:
    Dx12Texture(const TextureConfig& desc, Dx12Device* device);
    virtual ~Dx12Texture();
    virtual void init();

	Dx12RDMgr::Handle GetRtvHandle() const {
		return mRtvHandle;
	};

	Dx12RDMgr::Handle GetDsHandle() const {
		return mDsHandle;
	}

private:
    Dx12RDMgr::Handle mRtvHandle;
    Dx12RDMgr::Handle mDsHandle;
    D3D12_RENDER_TARGET_VIEW_DESC mRtDesc;
    D3D12_DEPTH_STENCIL_VIEW_DESC mDsDesc;
};

class Dx12Buffer : public Pegasus::Render::Buffer, public Dx12Resource
{
public:
    Dx12Buffer(const BufferConfig& desc, Dx12Device* device);
    Dx12Buffer(const GpuMemoryBlock& uploadBuffer, Dx12Device* device);
    virtual ~Dx12Buffer();
    virtual void init();
    virtual void* GetGpuPtr() { return GetDx12ResourceGpuPtr(); }
    size_t GetUploadSz() const { return m_uploadBufferSize; }

private:
    bool m_uploadBuffer = false;
    size_t m_uploadBufferSize = 0u;
};

class Dx12ResourceTable : public Pegasus::Render::ResourceTable
{
public:
    Dx12ResourceTable(const ResourceTableConfig& desc, Dx12Device* device);
    ~Dx12ResourceTable();
    const std::vector< Core::Ref<IResource> >& GetResources() { return GetConfig().resources; }
    const Dx12RDMgr::Table& GetTable() const { return mTable; }

private:
    Dx12RDMgr::Table mTable;   
    Dx12RDMgr* mRdMgr;
};

class Dx12RenderTarget : public Pegasus::Render::RenderTarget
{
public:
    friend class Dx12RenderContext;
    Dx12RenderTarget(const RenderTargetConfig& desc, Dx12Device* device);
    ~Dx12RenderTarget();

private:
    Dx12RDMgr::Table mTable;   
    Dx12RDMgr::Handle mDepthStencilHandle;
    Dx12RDMgr* mRdMgr;
};

typedef Core::Ref<Dx12Texture> Dx12TextureRef;
typedef Core::Ref<Dx12Buffer> Dx12BufferRef;
typedef Core::Ref<IResourceRef> Dx12ResourceRef;
typedef Core::Ref<Dx12ResourceTable> Dx12ResourceTableRef;
typedef Core::Ref<Dx12RenderTarget> Dx12RenderTargetRef;

}
}
