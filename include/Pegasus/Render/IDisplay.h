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
#include <Pegasus/Core/Formats.h>

namespace Pegasus { 
namespace Alloc {
    class IAllocator;
}
}

namespace Pegasus
{
namespace Render
{

class Texture;
class RenderTarget;
class IDevice;

struct DisplayConfig
{
    Pegasus::Os::WindowHandle windowHandle;
    Pegasus::Core::Format format = Pegasus::Core::Format::FORMAT_RGBA_8_UNORM;
    unsigned int buffering = 2u;
    unsigned int width = 128u;
    unsigned int height = 128u;
};

class IDisplay : public Core::RefCounted
{
public:
    virtual ~IDisplay() {}


    // New width, new height.
    virtual void Resize(unsigned int width, unsigned int height) = 0;
    virtual Texture* GetTexture() = 0;
    virtual RenderTarget* GetRenderTarget() = 0;

    Alloc::IAllocator* GetAllocator() { return mAllocator; }

    // Gets the config of this display
    const DisplayConfig& GetConfig() const { return mConfig; }

protected:
    IDisplay(const DisplayConfig& config, Alloc::IAllocator* alloc)
    : Pegasus::Core::RefCounted(alloc), mConfig(config), mAllocator(alloc) {}

    DisplayConfig mConfig;

private:
    Alloc::IAllocator * mAllocator;
};

typedef Core::Ref<IDisplay> IDisplayRef;

}
}


