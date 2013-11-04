/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IAllocator.h
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  General allocator interface.

#ifndef PEGASUS_MEMORY_IALLOCATOR_H
#define PEGASUS_MEMORY_IALLOCATOR_H

#include <stdlib.h>

namespace Pegasus {
namespace Memory {

//! Memory allocation flags
enum Flags
{
    PG_MEM_TEMP, //!< Temporal memory
    PG_MEM_PERM  //!< Permanent memory
};

//----------------------------------------------------------------------------------------

//! General allocator interface
class IAllocator
{
public:
    //!  Destructor
    ~IAllocator() {};


    //! Allocate a block of memory
    //! \param Size of the allocation, in bytes.
    //! \param flags Allocation flags.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Lne number of this allocation.
    //! \return Allocated memory.
    virtual void* Alloc(size_t size, Flags flags, const char* debugText = nullptr, const char* file = nullptr, unsigned int line = 0) const = 0;

    //! Allocate a block of memory for an array
    //! \param Size of the allocation, in bytes.
    //! \param flags Allocation flags.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Lne number of this allocation.
    //! \return Allocated memory.
    virtual void* AllocArray(size_t size, Flags flags, const char* debugText = nullptr, const char* file = nullptr, unsigned int line = 0) const = 0;

    //! Free a block of memory
    //! \param ptr Address of the memory.
    //! \param file File that made the free.
    //! \param line Line number of the free.
    virtual void Delete(void* ptr, const char* file = nullptr, unsigned int line = 0) const = 0;

    //! Free a block of memory for an array
    //! \param ptr Address of the memory.
    //! \param file File that made the free.
    //! \param line Line number of the free.
    virtual void DeleteArray(void* ptr, const char* file = nullptr, unsigned int line = 0) const = 0;
};


}   // namespace Memory
}   // namespace Pegasus

#endif  // PEGASUS_MEMORY_IALLOCATOR_H