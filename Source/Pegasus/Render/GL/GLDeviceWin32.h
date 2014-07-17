/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GLDeviceWin32.h
//! \author Kleber Garcia
//! \date   14th July 2014
//! \brief  Class that implements the platform specific / render api specific device

#ifndef PEGASUS_GLDEVICEWIN32_H
#define PEGASUS_GLDEVICEWIN32_H

#include "Pegasus/Render/IDevice.h"

namespace Pegasus
{
namespace Render
{

//! Windows specific device for open gl graphics api
class GLDeviceWin32 : public IDevice
{
public:
    //! Constructor
    //! \param config the configuration needed
    //! \param render allocator for internal allocations
    GLDeviceWin32(const DeviceConfig& config, Alloc::IAllocator * allocator);
    virtual ~GLDeviceWin32();
    
    //! Gets the pixel format descriptor for this device
    PIXELFORMATDESCRIPTOR* GetPixelFormatDescriptor();    

    //! Gets the context attributes for this opengl device
    int* GetContextAttributes();

};

}
}

#endif
