/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderContextConfig.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Config structure for a rendering context.

#ifndef PEGASUS_RENDER_RENDERCONTEXTCONFIG_H
#define PEGASUS_RENDER_RENDERCONTEXTCONFIG_H

#include "Pegasus/Render/RenderDefs.h"
#include "Pegasus/Render/IDevice.h"

namespace Pegasus {
namespace Render {

//! Configuration structure for a rendering context.
struct ContextConfig
{
public:
    //! Constructor
    ContextConfig() : mAllocator(nullptr), mDevice(nullptr), mDeviceContextHandle(0) {};

    //! Destructor
    ~ContextConfig() {};


    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this object
    IDevice * mDevice; //!< Parent device for this context (represents graphics device)
    DeviceContextHandle mDeviceContextHandle; //!< Opaque context handle
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_RENDERCONTEXTCONFIG_H
