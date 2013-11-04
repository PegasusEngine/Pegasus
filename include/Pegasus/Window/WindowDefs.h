/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowDefs.h
//! \author David Worsham
//! \date   4th Junly 2013
//! \brief  Common typedefs for the windowing system.

#ifndef PEGASUS_WND_WINDOWDEFS_H
#define PEGASUS_WND_WINDOWDEFS_H

#include <stdint.h>

namespace Pegasus {
namespace Wnd {

//! Module handle opaque type
//! Can be converted to a pointer
//! Module is an OS-agnostic concept for a chunk of executable code
//! This can be an .exe or .dll (on Windows), or a .so (on Linux)
typedef uintptr_t ModuleHandle;

//! Window handle opaque type
//! Can be converted to a pointer
typedef uintptr_t WindowHandle;


}   // namespace Wnd
}   // namespace Pegasus

#endif  // PEGASUS_WND_WINDOWDEFS_H
