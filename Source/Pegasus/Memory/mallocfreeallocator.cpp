/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   mallocfreeallocator.cpp
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Basic allocator using stdC malloc and free from the system heap.

#include "Pegasus/Memory/mallocfreeallocator.h"
#include "Pegasus/Memory/Newdelete.h"

namespace Pegasus {
namespace Memory {

MallocFreeAllocator::MallocFreeAllocator()
{
}

//----------------------------------------------------------------------------------------

MallocFreeAllocator::~MallocFreeAllocator()
{
}

//----------------------------------------------------------------------------------------

void* MallocFreeAllocator::Alloc(size_t size, Flags flags, const char* debugText, const char* file, unsigned int line) const
{
    return malloc(size);
}

//----------------------------------------------------------------------------------------

void* MallocFreeAllocator::AllocArray(size_t size, Flags flags, const char* debugText, const char* file, unsigned int line) const
{
    return malloc(size);
}

//----------------------------------------------------------------------------------------

void MallocFreeAllocator::Delete(void* ptr, const char* file, unsigned int line) const
{
    if (ptr != nullptr)
    {
        free(ptr);
    }
}

//----------------------------------------------------------------------------------------

void MallocFreeAllocator::DeleteArray(void* ptr, const char* file, unsigned int line) const
{
    if (ptr != nullptr)
    {
        free(nullptr);
    }
}


}   // namespace Memory
}   // namespace Pegasus
