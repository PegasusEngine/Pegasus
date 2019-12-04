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

class Dx12Resource
{
    friend class Dx12ResourceTable;
public:
    Dx12Resource(const ResourceConfig& desc, Dx12Device* device);
    virtual ~Dx12Resource();
    ID3D12Resource* GetD3D() { return mData.resource; }
    D3D12_RESOURCE_STATES GetState (UINT subresourceIdx) const;
    void SetState(UINT subresourceIdx, D3D12_RESOURCE_STATES state);
    virtual void init();

    static Dx12Resource* GetDx12Resource(IResource* res);
    static const Dx12Resource* GetDx12Resource(const IResource* res);

    const ResourceConfig& GetResConfig() const { return mResConfig; }
    

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
    } mData;

    Dx12Device* mDevice;
    std::vector<D3D12_RESOURCE_STATES> mStates;

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
    virtual ~Dx12Buffer();
    virtual void init();
};

class Dx12ResourceTable : public Pegasus::Render::ResourceTable
{
public:
    friend class Dx12RenderContext;
    Dx12ResourceTable(const ResourceTableConfig& desc, Dx12Device* device);
    ~Dx12ResourceTable();
    const std::vector< Core::Ref<IResource> >& GetResources() { return GetConfig().resources; }

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
