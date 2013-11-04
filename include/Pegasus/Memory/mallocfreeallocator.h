/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MallocfreeAllocator.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Basic allocator using stdC malloc and free from the system heap.

#ifndef PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H
#define PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H

#include "Pegasus/Memory/IAllocator.h"

namespace Pegasus {
namespace Memory {

//! Basic allocator using stdC malloc and free from the system heap
class MallocFreeAllocator : public IAllocator
{
public:
    //! Constructor
    MallocFreeAllocator();

    //! Destructor
    virtual ~MallocFreeAllocator();


    // IAllocator interface
    virtual void* Alloc(size_t size, Flags flags, const char* debugText, const char* file, unsigned int line) const;
    virtual void* AllocArray(size_t size, Flags flags, const char* debugText, const char* file, unsigned int line) const;
    virtual void Delete(void* ptr, const char* file, unsigned int line) const;
    virtual void DeleteArray(void* ptr, const char* file, unsigned int line) const;
};


}   // namespace Memory
}   // namespace Pegasus

#endif  // PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H
