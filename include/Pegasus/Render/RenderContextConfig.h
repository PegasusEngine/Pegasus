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

namespace Pegasus {
namespace Render {

//! Configuration structure for a rendering context.
struct ContextConfig
{
public:
    //! Constructor
    ContextConfig() : mAllocator(nullptr), mDeviceContextHandle(0), mStartupContext(false) {};

    //! Destructor
    ~ContextConfig() {};


    Alloc::IAllocator* mAllocator; //!< Allocator to use when creating this object
    DeviceContextHandle mDeviceContextHandle; //!< Opaque context handle
    bool mStartupContext; //!< Startup context flag, indicating how the context should be created
};


}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_RENDERCONTEXTCONFIG_H
