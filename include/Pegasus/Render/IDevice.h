/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IDevice.h
//! \author Kleber Garcia
//! \date   7th July 2014
//! \brief  Class that encapsulates a drawing device

#include "Pegasus/Core/Shared/OsDefs.h"

#ifndef PEGASUS_RENDER_DEVICE
#define PEGASUS_RENDER_DEVICE

//forward declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace Render
{

//! Device configuration. Might vary per OS
struct DeviceConfig
{
    Os::ModuleHandle mModuleHandle; //! handle to the HINSTANCE if windows, handle to the proc if linux
};

class IDevice
{
public:
    //! destructor
    virtual ~IDevice(){}

    //! Gets the config of this device
    const DeviceConfig& GetConfig() const { return mConfig; }
    
    //! Gets the allocator of this device
    Alloc::IAllocator * GetAllocator() const { return mAllocator; }

    //! Global function that creats the device specific to a platform
    static IDevice * CreatePlatformDevice(const DeviceConfig& config, Alloc::IAllocator * allocator);

protected:
    //! constructor, which creates the device
    //! \param config configuration stored
    //! \param allocator the allocator for internal use
    IDevice(const DeviceConfig& config, Alloc::IAllocator * allocator) 
    : mConfig(config), mAllocator(allocator) {}

private:
    Alloc::IAllocator * mAllocator;
    DeviceConfig mConfig;
};

}
}

#endif
