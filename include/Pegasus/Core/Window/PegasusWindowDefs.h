/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PegasusWindowDefs.h
//! \author David Worsham
//! \date   4th Junly 2013
//! \brief  Common typedefs for the windowing system.

#ifndef PEGASUS_CORE_PEGASUSWINDOWDEFS_H
#define PEGASUS_CORE_PEGASUSWINDOWDEFS_H

//== Includes ====

//== Forward Declarations ====

//== Interface ====
namespace Pegasus {
namespace Core {

//! Application handle opaque type
//! Can be converted to a pointer
typedef unsigned int PG_HINSTANCE;

//! Window handle opaque type
//! Can be converted to a pointer
typedef unsigned int PG_HWND;


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_PEGASUSWINDOWDEFS_H
