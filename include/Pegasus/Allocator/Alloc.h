/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Alloc.h
//! \author David Worsham
//! \date   08th November 2013
//! \brief  Allocation macros for Pegasus.

#ifndef PEGASUS_ALLOC_ALLOC_H
#define PEGASUS_ALLOC_ALLOC_H

#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/internal/MacroImpl.h"

//! Macro for allocating memory
#define PG_NEW(_alloc, _cat, _debug_str, _flags) new(_alloc, _flags, _cat, _debug_str, __FILE__, __LINE__)

//! Macro for allocating memory, aligned
#define PG_NEW_ALIGN(_alloc, _align, _cat, _debug_str, _flags) new(_alloc, _align, _flags, _cat, _debug_str, __FILE__, __LINE__)

//! Macro for allocating memory, in an array
#define PG_NEW_ARRAY(_alloc, _cat, _debug_str, _flags, _type, _numElements) Pegasus::Alloc::internal::NewArray<_type>(_alloc, _flags, _numElements, _cat, _debug_str, __FILE__, __LINE__)

//! Macro for allocating memory, in an array aligned
#define PG_NEW_ARRAY_ALIGN(_alloc, _align, _cat, _debug_str, _flags, _type, _numElements) Pegasus::Alloc::internal::NewArrayAligned<_type>(_alloc, _align, _numElements, _cat, _flags, _debug_str, __FILE__, __LINE__)

//! Macro for freeing memory (to use with PG_NEW)
#define PG_DELETE(alloc, ptr) Pegasus::Alloc::internal::Delete(alloc, ptr);

//! Macro for freeing arrays of memory (to use with PG_NEW_ARRAY)
#define PG_DELETE_ARRAY(alloc, ptr) Pegasus::Alloc::internal::DeleteArray(alloc, ptr);


#endif  // PEGASUS_ALLOC_ALLOC_H
