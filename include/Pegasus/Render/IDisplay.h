/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IDisplay.h
//! \author Kleber Garcia
//! \date   8th July 2018
//! \brief  Utility class encapsulating swap chain / display behaviour.

#pragma once 

#include <Pegasus/Core/Shared/OsDefs.h>
#include <Pegasus/Core/RefCounted.h>
#include <Pegasus/Core/Ref.h>

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
    Pegasus::Os::WindowHandle windowHandle;
    unsigned int width;
    unsigned int height;
};

class IDisplay : public Core::RefCounted
{
public:
    virtual ~IDisplay() {}

    // Called at the beginning of a frame
    virtual void BeginFrame() = 0;

    // Absolutely last call if the frame. Performs a swap internally in the swap chain
    virtual void EndFrame() = 0;
    
    // New width, new height.
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    Alloc::IAllocator* GetAllocator() { return mAllocator; }

    // Gets the config of this display
    const DisplayConfig& GetConfig() const { return mConfig; }

protected:
    IDisplay(const DisplayConfig& config, Alloc::IAllocator* alloc)
    : Pegasus::Core::RefCounted(alloc), mConfig(config), mAllocator(alloc) {}

private:
    Alloc::IAllocator * mAllocator;
    DisplayConfig mConfig;
};

typedef Core::Ref<IDisplay> IDisplayRef;

}
}


