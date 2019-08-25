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

enum BindFlags : unsigned int
{
    BindFlags_Srv = 1,
    BindFlags_Cbv = 1 << 1,
    BindFlags_Uav = 1 << 2,
    BindFlags_Vb  = 1 << 3,
    BindFlags_Ib  = 1 << 4,
    BindFlags_Ds  = 1 << 5,
    BindFlags_Rt  = 1 << 6
};

enum BufferType : unsigned int
{
	BufferType_Default,
	BufferType_Structured,
	BufferType_Raw
};

enum ResourceUsage
{
    ResourceUsage_Static,
    ResourceUsage_Dynamic,
    ResourceUsage_Staging
};

enum TextureType
{
    TextureType_1d,
    TextureType_2d,
    TextureType_3d,
    TextureType_CubeMap
};

struct ResourceDesc
{
    std::string name;
    Core::Format format;
    unsigned int bindFlags;
    ResourceUsage usage;
};

struct TextureDesc : public ResourceDesc
{
    TextureType type;
    unsigned int width;
	unsigned int height;
    unsigned int depth;
    unsigned int mipLevels;
};

struct BufferDesc : public ResourceDesc
{
	unsigned int stride;
	unsigned int elementCount;
    BufferType bufferType;
};

struct ResourceTableDesc
{
    Dx12ResType type = Dx12_ResSrv;
    std::vector< Core::Ref<IResource> > resources;
};

class Dx12Resource
{
    friend class Dx12ResourceTable;
public:
    Dx12Resource(const ResourceDesc& desc, Dx12Device* device);
    virtual ~Dx12Resource();
    ID3D12Resource* GetD3D() { return mData.resource; }
    const ResourceDesc& GetDesc() const { return mDesc; }
    D3D12_RESOURCE_STATES GetState (UINT subresourceIdx) const;
    void SetState(UINT subresourceIdx, D3D12_RESOURCE_STATES state);
    virtual void init();

    static Dx12Resource* GetDx12Resource(IResource* res);
    static const Dx12Resource* GetDx12Resource(const IResource* res);

private:
    ResourceDesc mDesc;

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
};

class Dx12Texture : public Dx12Resource, public Pegasus::Render::Texture
{
public:
    Dx12Texture(const TextureDesc& desc, Dx12Device* device);
    virtual ~Dx12Texture();
    const TextureDesc& getTexDesc() const { return mDesc; }
    virtual void init();
private:
    Dx12RDMgr::Handle mRtvHandle;
    Dx12RDMgr::Handle mDsHandle;
    D3D12_RENDER_TARGET_VIEW_DESC mRtDesc;
    D3D12_DEPTH_STENCIL_VIEW_DESC mDsDesc;
    TextureDesc mDesc;
};

class Dx12Buffer : public Dx12Resource, public Pegasus::Render::Buffer
{
public:
    Dx12Buffer(const BufferDesc& desc, Dx12Device* device);
    virtual ~Dx12Buffer();
    const BufferDesc& getBufferDesc() const { return mDesc; }
    virtual void init();
private:
    BufferDesc mDesc;
};

class Dx12ResourceTable : public Pegasus::Render::ResourceTable
{
public:
    friend class Dx12RenderContext;
    Dx12ResourceTable(const ResourceTableDesc& desc, Dx12Device* device);
    ~Dx12ResourceTable();
    const ResourceTableDesc& GetDesc() const { return mDesc; }
    std::vector< Core::Ref<IResource> >& GetResources() { return mDesc.resources; }

private:
    Dx12RDMgr::Table mTable;   
	ResourceTableDesc mDesc;
    Dx12RDMgr* mRdMgr;
};

typedef Core::Ref<Dx12Texture> Dx12TextureRef;
typedef Core::Ref<Dx12Buffer> Dx12BufferRef;
typedef Core::Ref<IResourceRef> Dx12ResourceRef;
typedef Core::Ref<Dx12ResourceTable> Dx12ResourceTableRef;

}
}
