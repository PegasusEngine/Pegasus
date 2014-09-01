/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AstAllocator.h
//! \author Kleber Garcia
//! \date   1st september 2014
//! \brief  blockscript AST greedy allocator. All memory gets deallocated at once.

#ifndef AST_ALLOCATOR_H
#define AST_ALLOCATOR_H

#include "Pegasus/Allocator/IAllocator.h"

namespace Pegasus
{

namespace BlockScript
{

//! Ast allocator, following decorator pattern, helps recycling memory across BS sessions.
//! the AstAllocator destroys all memory at once, and only deallocates on a ForceFree call (or on destruction)
class AstAllocator : public Alloc::IAllocator
{
public:

    AstAllocator();
    virtual ~AstAllocator();
    
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

    Alloc::IAllocator* GetInternalAlloc() { return mAllocator; }

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