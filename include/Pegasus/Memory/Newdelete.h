/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   newdelete.h
//! \author Kleber Garcia
//! \date   11th October 2013
//! \brief  Global memory operator overrides

#ifndef PEGASUS_NEWDELETE_H
#define PEGASUS_NEWDELETE_H

#include "Pegasus/Memory/IAllocator.h"

//! Naked new operator
//! \param Size in bytes to alloc
void* operator new(size_t size);

//! Naked new operator
//! \param Size in bytes to alloc
void* operator new[](size_t size);

//! Pegasus new operator
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param flags Allocation flags
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
void* operator new(size_t size, Pegasus::Memory::IAllocator* allocator, Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line);

//! Pegasus array new operator
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param flags Allocation flags
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
void* operator new[] (size_t size, Pegasus::Memory::IAllocator* allocator, Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line);

//! Pegasus delete operator
//! \param pointer Address of memory to delete
void operator delete(void* pointer);

//! Pegasus array delete operator
//! \param pointer Address of memory to delete
void operator delete[](void* pointer);

#endif
