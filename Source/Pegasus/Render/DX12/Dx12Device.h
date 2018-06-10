/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Dx12Device.h
//! \author Kleber Garcia
//! \date   May 25th 2018
//! \brief  Class that implements the dx12 device

#ifndef PEGASUS_DX12DEVICE_H
#define PEGASUS_DX12DEVICE_H

#include <D3D12.h>
#include "Pegasus/Render/IDevice.h"


namespace Pegasus
{
namespace Render
{

//! Windows specific device for open gl graphics api
class Dx12Device : public IDevice
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    Dx12Device(const DeviceConfig& config, Alloc::IAllocator * allocator);
    virtual ~Dx12Device();
};

}
}


#endif
