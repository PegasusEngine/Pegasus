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

#pragma comment(lib, "d3d11")

#include "../Source/Pegasus/Render/DX11/DXDevice.h"

D3D_FEATURE_LEVEL gFeatureLevel[] = { D3D_FEATURE_LEVEL_11_0 };

namespace Pegasus
{
namespace Render
{

DXDevice::DXDevice(const DeviceConfig& config, Alloc::IAllocator * allocator)
: IDevice(config, allocator)
{
    mD3D11Device = NULL;
    mD3D11ImmContext = NULL;

    D3D_FEATURE_LEVEL targetFeatureLevel;
    D3D11CreateDevice(
        NULL, // use default adapter
        D3D_DRIVER_TYPE_HARDWARE, //use hardware capabilities
        NULL, // no software rasterizer handle

        D3D11_CREATE_DEVICE_SINGLETHREADED 
#if PEGASUS_GPU_DEBUG
        | D3D11_CREATE_DEVICE_DEBUG //enable debug flag on debug builds.
#endif
        ,
        gFeatureLevel,
        sizeof(gFeatureLevel) / sizeof(D3D_FEATURE_LEVEL),
        D3D11_SDK_VERSION,
        &mD3D11Device,
        &targetFeatureLevel,
        &mD3D11ImmContext
    );

    PG_ASSERTSTR(targetFeatureLevel == D3D_FEATURE_LEVEL_11_0, "DirectX 11 not supported on this machine! check the driver and your graphics card settings");
}

DXDevice::~DXDevice()
{
    mD3D11ImmContext->Release();
    mD3D11Device->Release();
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
