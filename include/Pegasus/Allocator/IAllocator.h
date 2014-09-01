/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IAllocator.h
//! \author David Worsham
//! \date   02nd November 2013
//! \brief  General allocator interface.

#ifndef PEGASUS_ALLOC_IALLOCATOR_H
#define PEGASUS_ALLOC_IALLOCATOR_H

namespace Pegasus {
namespace Alloc {

//! Memory allocation flags
enum Flags
{
    PG_MEM_TEMP, //!< Temporal memory
    PG_MEM_PERM  //!< Permanent memory
};

//! Memory alignment
typedef size_t Alignment;

//! Memory category
//! \warning -1 is reserved as an "invalid" category.  The rest are implementation-defined.
typedef int Category;

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
    //! \param category Allocation category.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Line number of this allocation.
    //! \return Allocated memory.
    virtual void* Alloc(size_t size, Flags flags, Category category = -1, const char* debugText = nullptr, const char* file = nullptr, unsigned int line = 0) = 0;

    //! Allocate a block of memory, aligned
    //! \param Size of the allocation, in bytes.
    //! \param align Allocation alignment, in bytes.
    //! \param flags Allocation flags.
    //! \param category Allocation category.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Line number of this allocation.
    //! \return Allocated memory.
    virtual void* AllocAlign(size_t size, Alignment align, Flags flags, Category category = -1, const char* debugText = nullptr, const char* file = nullptr, unsigned int line = 0) = 0;


    //! Free a block of memory
    //! \param ptr Address of the memory.
    virtual void Delete(void* ptr) = 0;
};


}   // namespace Alloc
}   // namespace Pegasus

#endif  // PEGASUS_ALLOC_IALLOCATOR_H
