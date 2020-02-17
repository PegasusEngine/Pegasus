#include <Pegasus/Render/IDevice.h>
#include <Pegasus/Allocator/Alloc.h>
#include <Pegasus/Render/JobBuilder.h>
#include "ResourceStateTable.h"
#include <functional>

#pragma once

namespace Pegasus
{
namespace Render
{
    struct CanonicalCmdListResult;
    class ResourceStateTable;
}
}

namespace Pegasus
{
namespace Render
{

class GpuWorkManager
{
public:
    using SubmitGpuWorkFn = std::function<GpuSubmitResult(const CanonicalCmdListResult& canonicalCmdListResult)>;

	GpuWorkManager(
        Alloc::IAllocator* allocator,
        ResourceStateTable& stateTable);
    GpuSubmitResult Submit(const RootJob& rootJob, SubmitGpuWorkFn workFn);
    void ReleaseWork(GpuWorkHandle workHandle);
    bool IsFinished(GpuWorkHandle workHandle);
    void Wait(GpuWorkHandle workHandle);
    void GarbageCollect();

private:
    Alloc::IAllocator* mAllocator;
    ResourceStateTable& mResourceStateTable;
};

template<class PlatDeviceT>
class ADevice : public IDevice
{
public:
    ADevice(const DeviceConfig& config, Pegasus::Alloc::IAllocator* allocator);
    virtual ~ADevice();

    virtual const DeviceConfig& GetConfig() const override { return mConfig; }
    virtual Pegasus::Alloc::IAllocator * GetAllocator() const override { return mAllocator; }
    virtual BufferRef CreateBuffer(const BufferConfig& config) override;
    virtual TextureRef CreateTexture(const TextureConfig& config) override;
    virtual RenderTargetRef CreateRenderTarget(const RenderTargetConfig& config) override;
    virtual ResourceTableRef CreateResourceTable(const ResourceTableConfig& config) override;
    virtual GpuPipelineRef CreateGpuPipeline() override;
    virtual IDisplayRef CreateDisplay(const DisplayConfig& displayConfig);
    virtual ResourceStateTable* GetResourceStateTable() const override { return mResourceStateTable; }
    
    virtual GpuSubmitResult Submit(const RootJob& rootJob) override;
    virtual void ReleaseWork(GpuWorkHandle workHandle) override;
    virtual bool IsFinished(GpuWorkHandle workHandle) override;
    virtual void Wait(GpuWorkHandle workHandle) override;
    virtual void GarbageCollect() override;


protected:
    Pegasus::Alloc::IAllocator* mAllocator;
    DeviceConfig mConfig;
    ResourceStateTable* mResourceStateTable;
    GpuWorkManager mGpuWorkManager;
};


template<class PlatDeviceT>
ADevice<PlatDeviceT>::ADevice(const DeviceConfig& config, Pegasus::Alloc::IAllocator* allocator)
: mConfig(config)
, mAllocator(allocator)
, mResourceStateTable(PG_NEW(allocator, -1, "ResourceStateTable", Pegasus::Alloc::PG_MEM_PERM) ResourceStateTable)
, mGpuWorkManager(mAllocator, *mResourceStateTable)
{
    
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

template<class PlatDeviceT>
IDisplayRef ADevice<PlatDeviceT>::CreateDisplay(const DisplayConfig& displayConfig)
{
    return static_cast<PlatDeviceT*>(this)->InternalCreateDisplayConfig(displayConfig);
}

template<class PlatDeviceT>
GpuSubmitResult ADevice<PlatDeviceT>::Submit(const RootJob& rootJob)
{
   return mGpuWorkManager.Submit(rootJob, [this](const CanonicalCmdListResult& result){ return static_cast<PlatDeviceT*>(this)->InternalSubmit(result); });
}

template<class PlatDeviceT>
void ADevice<PlatDeviceT>::ReleaseWork(GpuWorkHandle workHandle)
{
    //mGpuWorkManager.ReleaseWork(workHandle);
}

template<class PlatDeviceT>
bool ADevice<PlatDeviceT>::IsFinished(GpuWorkHandle workHandle)
{
	return false;
    //return mGpuWorkManager.IsFinished(workHandle);
}

template<class PlatDeviceT>
void ADevice<PlatDeviceT>::Wait(GpuWorkHandle workHandle)
{
   // mGpuWorkManager.Wait(workHandle);
}

template<class PlatDeviceT>
void ADevice<PlatDeviceT>::GarbageCollect()
{
   // mGpuWorkManager.GarbageCollect();
}


}
}
