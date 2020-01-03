#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Allocator/Alloc.h>
#include "ResourceStateTable.h"

#pragma once

namespace Pegasus
{
namespace Render
{
    class ResourceStateTable;
}
}

namespace Pegasus
{
namespace Render
{

template<class PlatDeviceT>
class ADevice : public IDevice
{
public:
    ADevice(const DeviceConfig& config, Alloc::IAllocator* allocator);
    virtual ~ADevice();

    virtual const DeviceConfig& GetConfig() const override { return mConfig; }
    virtual Alloc::IAllocator * GetAllocator() const override { return mAllocator; }
    virtual BufferRef CreateBuffer(const BufferConfig& config) override;
    virtual TextureRef CreateTexture(const TextureConfig& config) override;
    virtual RenderTargetRef CreateRenderTarget(const RenderTargetConfig& config) override;
    virtual ResourceTableRef CreateResourceTable(const ResourceTableConfig& config) override;
    virtual GpuPipelineRef CreateGpuPipeline() override;
    virtual ResourceStateTable* GetResourceStateTable() const override { return mResourceStateTable; }

protected:
    Alloc::IAllocator * mAllocator;
    DeviceConfig mConfig;
    ResourceStateTable* mResourceStateTable;
};

template<class PlatDeviceT>
ADevice<PlatDeviceT>::ADevice(const DeviceConfig& config, Alloc::IAllocator* allocator)
: mConfig(config), mAllocator(allocator)
{
    mResourceStateTable = PG_NEW(allocator, -1, "ResourceStateTable", Pegasus::Alloc::PG_MEM_PERM) ResourceStateTable;
}

template<class PlatDeviceT>
ADevice<PlatDeviceT>::~ADevice()
{
    PG_DELETE(mAllocator, mResourceStateTable);
}

template<class PlatDeviceT>
BufferRef ADevice<PlatDeviceT>::CreateBuffer(const BufferConfig& config)
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateBuffer(config);
}

template<class PlatDeviceT>
TextureRef ADevice<PlatDeviceT>::CreateTexture(const TextureConfig& config)
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateTexture(config);
}

template<class PlatDeviceT>
RenderTargetRef ADevice<PlatDeviceT>::CreateRenderTarget(const RenderTargetConfig& config)
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateRenderTarget(config);
}

template<class PlatDeviceT>
ResourceTableRef ADevice<PlatDeviceT>::CreateResourceTable(const ResourceTableConfig& config)
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateResourceTable(config);
}

template<class PlatDeviceT>
GpuPipelineRef ADevice<PlatDeviceT>::CreateGpuPipeline()
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateGpuPipeline();
}

}
}
