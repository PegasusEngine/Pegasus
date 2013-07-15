/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   WindowDefs.h
//! \author David Worsham
//! \date   4th Junly 2013
//! \brief  Common typedefs for the windowing system.

#ifndef PEGASUS_WINDOWDEFS_H
#define PEGASUS_WINDOWDEFS_H

#include <stdint.h>

namespace Pegasus {

//! Application handle opaque type
//! Can be converted to a pointer
typedef uintptr_t ApplicationHandle;

//! Window handle opaque type
//! Can be converted to a pointer
typedef uintptr_t WindowHandle;


}   // namespace Pegasus

#endif  // PEGASUS_WINDOWDEFS_H
