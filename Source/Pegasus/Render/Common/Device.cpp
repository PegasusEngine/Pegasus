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

}
}

