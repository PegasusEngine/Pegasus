/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IDisplay.h
//! \author Kleber Garcia
//! \date   8th July 2018
//! \brief  Utility class encapsulating swap chain / display behaviour.

#include <Pegasus/Core/Shared/OsDefs.h>

namespace Pegasus { 
namespace Alloc {
    class IAllocator;
}
}

namespace Pegasus
{
namespace Render
{

class IDevice;

struct DisplayConfig
{
    Os::ModuleHandle moduleHandle; //! handle to the HINSTANCE if windows, handle to the proc if linux
    IDevice* device;
    unsigned int width;
    unsigned int height;
};

class IDisplay
{
public:
    virtual ~IDisplay() {}

    // Called at the beginning of a frame
    virtual void BeginFrame() = 0;

    // Absolutely last call if the frame. Performs a swap internally in the swap chain
    virtual void EndFrame() = 0;
    
    // New width, new height.
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    // Gets the config of this display
    const DisplayConfig& GetConfig() const { return mConfig; }

    //Global function that creates the display specific to a platform.
    static IDisplay* CreatePlatformDisplay(const DisplayConfig& displayConfig, Alloc::IAllocator* alloc);

protected:
    IDisplay(const DisplayConfig& config, Alloc::IAllocator* alloc)
    : mConfig(config), mAllocator(alloc) {}

private:
    Alloc::IAllocator * mAllocator;
    DisplayConfig mConfig;
};

}
}


