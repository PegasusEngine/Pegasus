/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Memory.h
//! \author Kleber Garcia
//! \date   14th October 2013
//! \brief  Memory framework

#ifndef PEGASUS_MEMORY_MEMORY_H
#define PEGASUS_MEMORY_MEMORY_H

#include "Pegasus/Memory/Newdelete.h"

//! Macro for allocating memory
#define PG_NEW(alloc, debug_str, flags) new(alloc, flags, debug_str, __FILE__, __LINE__)
//! Macro for allocating memory
#define PG_NEW_ARRAY(alloc, debug_str, flags, type, numElements) new(alloc, flags, debug_str, __FILE__, __LINE__) type[numElements]
//! Macro for freeing memory (to use with PG_NEW)
#define PG_DELETE(alloc, ptr) alloc->Delete(ptr, __FILE__, __LINE__)
//! Macro for freeing arrays of memory (to use with PG_NEW_ARRAY)
#define PG_DELETE_ARRAY(alloc, ptr) alloc->DeleteArray(ptr, __FILE__, __LINE__)

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Memory {

//! Get the global allocator
//! \return Global allocator, for the global heap.
IAllocator* GetGlobalAllocator();

//! Get the core allocator
//! \return Core allocator.
IAllocator* GetCoreAllocator();

//! Get the render allocator
//! \return Render allocator.
IAllocator* GetRenderAllocator();

//! Get the window/app allocator
//! \return Window/app allocator.
IAllocator* GetWindowAllocator();

}   // namespace Memory
}   // namepsace Pegasus

#endif // PEGASUS_MEMORY_MEMORY_H
