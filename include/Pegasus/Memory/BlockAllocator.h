/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockAllocator.h
//! \author Kleber Garcia
//! \date   February 8th 2015
//! \brief  BlockAllocator, greedy allocator. All memory gets deallocated at once.

#ifndef PEGASUS_BLOCK_ALLOCATOR_H
#define PEGASUS_BLOCK_ALLOCATOR_H

#include "Pegasus/Allocator/IAllocator.h"

namespace Pegasus
{

namespace Memory
{

//! Ast allocator, following decorator pattern, helps recycling memory across BS sessions.
//! the AstAllocator destroys all memory at once, and only deallocates on a ForceFree call (or on destruction)
class BlockAllocator : public Alloc::IAllocator
{
public:

    BlockAllocator();
    virtual ~BlockAllocator();
    
    //! decorate the underlying allocator. The 
    //! \param alloc the allocator querying underneath
    void Initialize(int pageSize, Alloc::IAllocator* alloc);

    //! Allocate a block of memory
    //! \param Size of the allocation, in bytes.
    //! \param flags Allocation flags.
    //! \param category Allocation category.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Line number of this allocation.
    //! \return Allocated memory.
    virtual void* Alloc(
            size_t size, 
            Alloc::Flags flags, 
            Alloc::Category category = -1, 
            const char* debugText = nullptr, 
            const char* file = nullptr, 
            unsigned int line = 0
    );

    //! Allocate a block of memory, aligned
    //! \param Size of the allocation, in bytes.
    //! \param align Allocation alignment, in bytes.
    //! \param flags Allocation flags.
    //! \param category Allocation category.
    //! \param debugText Debug name for this allocation.
    //! \param file File that made this allocation.
    //! \param line Line number of this allocation.
    //! \return Allocated memory.
    virtual void* AllocAlign(
        size_t size, 
        Alloc::Alignment align, 
        Alloc::Flags flags, 
        Alloc::Category category = -1, 
        const char* debugText = nullptr, 
        const char* file = nullptr, 
        unsigned int line = 0
    );


    //! Free a block of memory
    //! \param ptr Address of the memory.
    virtual void Delete(void* ptr);

    //! Resets the memory counter, but does not destroy the allocated memory. Use this for iteration on recompilation of block scripts
    void Reset();

    //! Frees the memory and resets the counters. Use this when doing a garbage collection pass or when memory must be freed
    void FreeMemory();

    //! \return the allocator
    Alloc::IAllocator* GetInternalAlloc() { return mAllocator; }

    //! \return gets the page memory
    char** const GetMemoryPages() const { return mMemoryPages; }

    //! \return gets the page memory
    char** GetMemoryPages() { return mMemoryPages; }

    //! \return gets the page memory
    char** const GetMemoryPagesConst() const { return mMemoryPages; }

    //! \return gets the memory size
    int GetMemorySize() const { return (int)mMemorySize; }

    int GetPageSize() const { return mPageSize; }

private:
    static const int sPageIncrement; 

    Alloc::IAllocator* mAllocator;
    size_t mMemorySize;
    int    mMemoryPageListCount;
    int    mMemoryPageListSize;
    char** mMemoryPages;
    int    mPageSize;
    
};



}
}

#endif
