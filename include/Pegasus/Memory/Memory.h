/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memory.h
//! \author	Kleber Garcia
//! \date	14th October 2013
//! \brief	Memory framework


#ifndef PEGASUS_MEMORY_H
#define PEGASUS_MEMORY_H

#include "Pegasus/Memory/newdelete.h"

#define PG_CORE_NEW(debug_str, flags) new(nullptr/*null allocator for now*/, flags, debug_str, __FILE__, __LINE__)
#define PG_GRAPHICS_NEW(debug_str, flags) new(nullptr/*null allocator for now*/, flags, debug_str, __FILE__, __LINE__)
#define PG_DEBUG_NEW(debug_str, flags) new(nullptr/*null allocator for now*/, flags, debug_str, __FILE__, __LINE__)
#define PG_DELETE delete
#define PG_DELETE_ARRAY delete[]
#endif