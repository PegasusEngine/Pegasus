/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   NewDelete.h
//! \author Kleber Garcia
//! \date   11th October 2013
//! \brief  Global memory operator overrides

#ifndef PEGASUS_ALLOC_NEWDELETE_H
#define PEGASUS_ALLOC_NEWDELETE_H

#include "Pegasus/Allocator/IAllocator.h"
/*
//! Naked new operator
//! \param Size in bytes to alloc
void* operator new(size_t size);

//! Naked new operator
//! \param Size in bytes to alloc
void* operator new[](size_t size);
*/

//! Pegasus new operator
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \note Don't call this directly, use the macro
void* operator new(size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus new operator, aligned
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param align Allocation alignment
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \note Don't call this directly, use the macro
void* operator new(size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus array new operator
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_NEW_ARRAY instead
void* operator new[] (size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus array new operator
//! \param size Size in bytes to alloc
//! \param allocator Allocator to use for this operation
//! \param align Allocation alignment
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_NEW_ARRAY instead
void* operator new[] (size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

/*
//! Pegasus delete operator
//! \param pointer Address of memory to delete
//! \todo Make sure this function is called properly in a unit test
void operator delete(void* pointer);
*/

//! Pegasus delete operator
//! \param pointer Address of memory to delete
//! \param allocator Allocator used by the corresponding new
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_DELETE instead
void operator delete(void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus delete operator, aligned
//! \param pointer Address of memory to delete
//! \param allocator Allocator used by the corresponding new
//! \param align Allocation alignment
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_DELETE instead
void operator delete(void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus array delete operator
//! \param pointer Address of memory to delete
//! \param allocator Allocator used by the corresponding new
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_DELETE_ARRAY instead
void operator delete[](void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);

//! Pegasus array delete operator, aligned
//! \param pointer Address of memory to delete
//! \param allocator Allocator used by the corresponding new
//! \param align Allocation alignment
//! \param flags Allocation flags
//! \param cat Allocation category
//! \param debugText Name for this allocation, doesn't exist in REL mode
//! \param file File this allocation was made in, __FILE__
//! \param line Line # this allocation was made on, __LINE__
//! \warning Never call this, it will force a crash.  Use PG_DELETE_ARRAY instead
void operator delete[](void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line);


#endif  // PEGASUS_ALLOC_NEWDELETE_H
