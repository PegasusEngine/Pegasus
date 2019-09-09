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
#include <Pegasus/Core/RefCounted.h>
#include <Pegasus/Allocator/Alloc.h>
#include "../Dx12/Dx12Device.h"
#include "../Dx12/Dx12Display.h"
#include "ResourceStateTable.h"


namespace Pegasus
{
namespace Render
{

IDevice::IDevice(const DeviceConfig& config, Alloc::IAllocator* allocator)
: mAllocator(allocator), mConfig(config)
{
    mResourceStateTable = PG_NEW(allocator, -1, "ResourceStateTable", Pegasus::Alloc::PG_MEM_PERM) ResourceStateTable;
}

IDevice::~IDevice()
{
    PG_DELETE(mAllocator, mResourceStateTable);
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

GpuPipelineRef IDevice::CreateGpuPipeline()
{
    return InternalCreateGpuPipeline();
}

IResource::IResource(IDevice* device, ResourceType resourceType)
: mResourceStateTable(device->GetResourceStateTable()),
  mAllocator(device->GetAllocator()),
  mResourceType(resourceType),
  RefCounted(device->GetAllocator()),
  mStateId(0u)
{
    mStateId = mResourceStateTable->CreateStateSlot();
}

IResource::~IResource()
{
    mResourceStateTable->RemoveStateSlot(mStateId);
}

GpuPipeline::GpuPipeline(IDevice* device)
: Core::RefCounted(device->GetAllocator())
{
#if PEGASUS_USE_EVENTS
    SetEventUserData(nullptr);
    SetEventListener(nullptr);
#endif
}


//! platform implementation of device
IDevice * IDevice::CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
{
    if (config.platform == DevicePlat::Dx12)
    {
        return PG_NEW(allocator, -1, "Dx12Device", Pegasus::Alloc::PG_MEM_PERM) Dx12Device(config, allocator);
    }

    return nullptr;
}

IDisplay* IDisplay::CreatePlatformDisplay(const DisplayConfig& displayConfig, Alloc::IAllocator* alloc)
{
    if (displayConfig.device != nullptr)
    {
        if (displayConfig.device->GetConfig().platform == DevicePlat::Dx12)
            return PG_NEW(alloc, -1, "Dx12Display", Pegasus::Alloc::PG_MEM_PERM) Dx12Display(displayConfig, alloc);
    }
    
    return nullptr;
}


}
}

