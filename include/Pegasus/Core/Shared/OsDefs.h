/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RenderDefs.h
//! \author David Worsham / Kleber Garcia
//! \date   7th July 2014
//! \brief  Common opaque typedefs for OS specific calls

#ifndef PEGASUS_OS_DEFS_H
#define PEGASUS_OS_DEFS_H

namespace Pegasus {
namespace Os {

//! Module handle opaque type
//! Can be converted to a pointer
//! Module is an OS-agnostic concept for a chunk of executable code
//! This can be an .exe or .dll (on Windows), or a .so (on Linux)
typedef uintptr_t ModuleHandle;

//! Window handle opaque type
//! Can be converted to a pointer
typedef uintptr_t WindowHandle;

}
}

#endif
