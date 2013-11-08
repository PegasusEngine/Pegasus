/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MacroImpl.h
//! \author David Worsham
//! \date   08th November 2013
//! \brief  Private implementation for PG_NEW*, PG_DELETE* macros.

#ifndef PEGASUS_ALLOCATOR_MACROIMPL_H
#define PEGASUS_ALLOCATOR_MACROIMPL_H

#include "Pegasus/Allocator/internal/NewDelete.h"
#include <new>

namespace Pegasus {
namespace Alloc {
namespace internal {

//! Allocates a new array of objects, initializing all of the objects with their default constructor
//! \param T Type of the objects.
//! \param alloc Allocator to use when grabbing memory.
//! \param flags Allocation flags.
//! \param count Number of elements in the array.
//! \param category Allocation category.
//! \param debug_str Debug name for the allocation.
//! \param file Filename of the allocation.
//! \param line Line number of the allocation.
template <typename T>
inline T* NewArray(const IAllocator* alloc, Flags flags, unsigned int count, Category category, const char* debug_str, const char* file, unsigned int line)
{
    // Grab memory, and request an extra int for the size
    const unsigned int blockSize = sizeof(T) * count + sizeof(unsigned int);
    T* block = (T*) alloc->Alloc(blockSize, flags, category, debug_str, file, line);
    T* arrayPtr = (T*) (((unsigned int*) block) + 1);

    // Cache the size at the front
    *((unsigned int*) block) = count;

    // Init the array with placement new from beginning to end
    for (unsigned int i = 0; i < count; i++)
    {
        new(arrayPtr + i) T();
    }

    return arrayPtr;
}

//----------------------------------------------------------------------------------------

//! Allocates a new array of objects, initializing all of the objects with their default constructor
//! \param T Type of the objects.
//! \param alloc Allocator to use when grabbing memory.
//! \param align Alignment of the allocation, in bytes.
//! \param flags Allocation flags.
//! \param count Number of elements in the array.
//! \param category Allocation category.
//! \param debug_str Debug name for the allocation.
//! \param file Filename of the allocation.
//! \param line Line number of the allocation.
template <typename T>
inline T* NewArrayAligned(const IAllocator* alloc, Alignment align, Flags flags, unsigned int count, Category category, const char* debug_str, const char* file, unsigned int line)
{
    // Grab memory, and request an extra int for the size
    const unsigned int blockSize = sizeof(T) * count + sizeof(unsigned int);
    T* block = (T*) alloc->AllocAlign(blockSize, align, flags, category, debug_str, file, line);
    T* arrayPtr = (T*) (((unsigned int*) block) + 1);

    // Cache the size at the front
    *((unsigned int*) block) = count;

    // Init the array with placement new from beginning to end
    for (unsigned int i = 0; i < count, i++)
    {
        new(arrayPtr + i) T();
    }

    return arrayPtr;
}

//----------------------------------------------------------------------------------------

//! Delete an object, destroying it using the destructor
//! \param T Type of the object.
//! \param alloc Allocator to use when freeing the memory.
//! \param ptr Pointer to the object.
template <typename T>
inline void Delete(const IAllocator* alloc, T* ptr)
{
    if (ptr != nullptr)
    {
        ptr->~T();
        alloc->Delete(ptr);
    }
}

//----------------------------------------------------------------------------------------

//! Delete an array of objects, destroying them using their destructors
//! \param T Type of the objects.
//! \param alloc Allocator to use when freeing the memory.
//! \param arrayPtr Pointer to the array.
template <typename T>
inline void DeleteArray(const IAllocator* alloc, T* arrayPtr)
{
    if (arrayPtr != nullptr)
    {
        // Grab block and count
        // Count is at the front of the block
        void* block = ((unsigned int*) arrayPtr) - 1;
        unsigned int count = *((unsigned int*) block);

        // Destruct from the end of the array to the beginning
        // Then release memory
        T* arrayEnd = arrayPtr + count;
        while (arrayEnd != arrayPtr)
        {
            (arrayEnd - 1)->~T();
            arrayEnd--;
        }
        alloc->Delete(block);
    }
}


}   // namespace internal
}   // namespace Alloc
}   // namespace Pegasus

#endif  // PEGASUS_ALLOCATOR_MACROIMPL_H
