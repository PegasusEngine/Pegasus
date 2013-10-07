/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderDefs.h
//! \author David Worsham
//! \date   15th July 2013
//! \brief  Common typedefs for the rendering subsystem.

#ifndef PEGASUS_RENDER_RENDERDEFS_H
#define PEGASUS_RENDER_RENDERDEFS_H

#include <stdint.h>


namespace Pegasus {
namespace Render {

//! Device context handle opaque type
//! Can be converted to a pointer
typedef uintptr_t DeviceContextHandle;

//! Render context handle opaque type
//! Can be converted to a pointer
typedef uintptr_t RenderContextHandle;

}   // namespace Render
}   // namespace Pegasus

#endif  // PEGASUS_RENDER_RENDERDEFS_H
