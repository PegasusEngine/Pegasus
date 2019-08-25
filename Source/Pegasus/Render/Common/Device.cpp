/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Device.cpp
//! \author Kleber Garcia
//! \date   August 24th, 2019
//! \brief  Concrete device interface functions

#include <Pegasus/Render/IDevice.h>


namespace Pegasus
{
namespace Render
{

IDevice::IDevice(const DeviceConfig& config, Alloc::IAllocator* allocator)
: mAllocator(allocator), mConfig(config), mResourceLookupTable(nullptr)
{
}

BufferRef IDevice::CreateBuffer(const BufferConfig& config)
{
    return InternalCreateBuffer(config);
}

TextureRef IDevice::CreateTexture(const TextureConfig& config)
{
    return InternalCreateTexture(config);
}

RenderTargetRef IDevice::CreateRenderTarget(const RenderTargetConfig& config)
{
    return InternalCreateRenderTarget(config);
}

ResourceTableRef IDevice::CreateResourceTable(const ResourceTableConfig& config)
{
    return InternalCreateResourceTable(config);
}

GpuPipelineRef IDevice::CreateGpuPipeline(const GpuPipelineConfig& config)
{
    return InternalCreateGpuPipeline(config);
}

IResource::IResource(IDevice* device, ResourceType resourceType)
: mResourceLookupTable(device->GetResourceTable()),
  mAllocator(device->GetAllocator()),
  mResourceType(resourceType),
  RefCounted(device->GetAllocator())
{
}

IResource::~IResource()
{
}


}
}

