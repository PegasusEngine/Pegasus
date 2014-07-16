/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXDevice.h
//! \author Kleber Garcia
//! \date   17th of July 2014
//! \brief  Class that implements the platform specific / render api specific device

#ifndef PEGASUS_DXDEVICE_H
#define PEGASUS_DXDEVICE_H

#include "Pegasus/Render/IDevice.h"

namespace Pegasus
{
namespace Render
{

//! Windows specific device for open gl graphics api
class DXDevice : public IDevice
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    DXDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);
    ~DXDevice();
};

}
}


#endif
