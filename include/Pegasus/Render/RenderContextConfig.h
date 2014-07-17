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

#include "Pegasus/Core/Shared/OsDefs.h"
#include "Pegasus/Render/IDevice.h"

namespace Pegasus {
namespace Render {

//! Configuration structure for a rendering context.
struct ContextConfig
{
public:
    //! Constructor
    ContextConfig() : mAllocator(nullptr), mDevice(nullptr), mOwnerWindowHandle(0) {};

    //! Destructor
    ~ContextConfig() {};


    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this object
    IDevice * mDevice; //!< Parent device for this context (represents graphics device)
    Os::WindowHandle mOwnerWindowHandle; //!< Opaque owner window handle
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_RENDERCONTEXTCONFIG_H
