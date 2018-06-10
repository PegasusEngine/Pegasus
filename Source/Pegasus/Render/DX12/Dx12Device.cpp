/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Device.cpp
//! \author Kleber Garcia
//! \date   17th of July 2014
//! \brief  Class that implements the platform specific / render api specific device

#if PEGASUS_GAPI_DX12

#pragma comment(lib, "d3d12")

#include "Dx12Device.h"

namespace Pegasus
{
namespace Render
{

Dx12Device::Dx12Device(const DeviceConfig& config, Alloc::IAllocator * allocator)
: IDevice(config, allocator)
{
}

Dx12Device::~Dx12Device()
{
}

//! platform implementation of device
IDevice * IDevice::CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
{
    return nullptr;
}

}//namespace Render
}//namespace Pegasus

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
