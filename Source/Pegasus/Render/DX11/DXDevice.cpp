/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXDevice.cpp
//! \author Kleber Garcia
//! \date   17th of July 2014
//! \brief  Class that implements the platform specific / render api specific device

#if PEGASUS_GAPI_DX

#include "../Source/Pegasus/Render/DX11/DXDevice.h"

namespace Pegasus
{
namespace Render
{

DXDevice::DXDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
: IDevice(config, allocator)
{
}

DXDevice::~DXDevice()
{
}

//! platform implementation of device
IDevice * IDevice::CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
{
    return PG_NEW(allocator, -1, "Device", Pegasus::Alloc::PG_MEM_PERM) DXDevice(config, allocator);
}

}//namespace Render
}//namespace Pegasus

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
