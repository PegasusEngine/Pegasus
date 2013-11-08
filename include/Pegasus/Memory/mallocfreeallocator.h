/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MallocFreeAllocator.h
//! \author David Worsham
//! \date   02nd November 2013
//! \brief  Basic allocator using stdC malloc and free from the system heap.

#ifndef PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H
#define PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H

#include "Pegasus/Allocator/IAllocator.h"

namespace Pegasus {
namespace Memory {

//! Basic allocator using stdC malloc and free from the system heap
class MallocFreeAllocator : public Alloc::IAllocator
{
public:
    //! Constructor
    //! \param allocId ID to use for this allocator.  Should be "Unique"
    MallocFreeAllocator(unsigned int allocId);

    //! Destructor
    virtual ~MallocFreeAllocator();


    // IAllocator interface
    virtual void* Alloc(size_t size, Alloc::Flags flags, Alloc::Category category, const char* debugText, const char* file, unsigned int line) const;
    virtual void* AllocAlign(size_t size, Alloc::Alignment align, Alloc::Flags flags, Alloc::Category category, const char* debugText, const char* file, unsigned int line) const;
    virtual void Delete(void* ptr) const;

private:
    // No copies allowed
    PG_DISABLE_COPY(MallocFreeAllocator);

    unsigned int mAllocId; //!< "Unique" allocator ID
};


}   // namespace Memory
}   // namespace Pegasus

#endif  // PEGASUS_MEMORY_MALLOCFREEALLOCATOR_H
