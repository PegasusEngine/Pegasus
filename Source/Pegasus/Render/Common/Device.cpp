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
    return InternalCreateBuffer(mResourceLookupTable, config);
}

TextureRef IDevice::CreateTexture(const TextureConfig& config)
{
    return InternalCreateTexture(mResourceLookupTable, config);
}

RenderTargetRef IDevice::CreateRenderTarget(const RenderTargetConfig& config)
{
    return InternalCreateRenderTarget(mResourceLookupTable, config);
}

ResourceTableRef IDevice::CreateResourceTable(const ResourceTableConfig& config)
{
    return InternalCreateResourceTable(mResourceLookupTable, config);
}

GpuPipelineRef IDevice::CreateGpuPipeline(const GpuPipelineConfig& config)
{
    return InternalCreateGpuPipeline(mResourceLookupTable, config);
}

IResource::IResource(Alloc::IAllocator* allocator, ResourceType resourceType, ResourceLookupTable* rlt)
: mResourceLookupTable(rlt), mAllocator(allocator), mResourceType(resourceType), RefCounted(allocator)
{
}

IResource::~IResource()
{
}


}
}

