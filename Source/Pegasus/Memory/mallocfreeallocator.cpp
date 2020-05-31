/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MallocFreeAllocator.cpp
//! \author David Worsham
//! \date   02nd November 2013
//! \brief  Basic allocator using stdC malloc and free from the system heap.

#include "Pegasus/Memory/MallocFreeAllocator.h"

namespace Pegasus {
namespace Memory {

MallocFreeAllocator::MallocFreeAllocator(unsigned int allocId)
    : mAllocId(allocId)
{
}

//----------------------------------------------------------------------------------------

MallocFreeAllocator::~MallocFreeAllocator()
{
}

//----------------------------------------------------------------------------------------

void* MallocFreeAllocator::Alloc(size_t size, Alloc::Flags flags, Alloc::Category category, const char* debugText, const char* file, unsigned int line)
{
    //! \todo Platform-specific allocs
    // Grab the chunk with an extra int at the front for the allocator ID
    uintptr_t chunkSize = size + sizeof(unsigned int);
    void* chunk = malloc(chunkSize);
    void* ret = ((unsigned int*) chunk) + 1;

    // Cache the ID
    *((unsigned int*) chunk) = mAllocId;

    return ret;
}

//----------------------------------------------------------------------------------------

void* MallocFreeAllocator::AllocAlign(size_t size, Alloc::Alignment align, Alloc::Flags flags, Alloc::Category category, const char* debugText, const char* file, unsigned int line)
{
    //! \todo Platform-specific allocs
    // Grab the chunk with an extra int at the front for the allocator ID
    uintptr_t chunkSize = size + sizeof(unsigned int);
    void* chunk = malloc(chunkSize);
    void* ret = ((unsigned int*) chunk) + 1;

    // Cache the ID
    *((unsigned int*) chunk) = mAllocId;

    return ret;
}

//----------------------------------------------------------------------------------------

void MallocFreeAllocator::Delete(void* ptr)
{
    if (ptr != nullptr)
    {
        // Grab the chunk and allocator ID
        void* chunk = ((unsigned int*) ptr) - 1;
        const unsigned int chunkId = *((unsigned int*) chunk);

        // Allocator integrity check
        PG_ASSERTSTR(chunkId == mAllocId, "Allocation freed from a different allocator than it was alloced in!  Memory corruption may follow...");

        free(chunk);
    }
}


}   // namespace Memory
}   // namespace Pegasus
