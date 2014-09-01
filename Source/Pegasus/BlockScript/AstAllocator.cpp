/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AstAllocator.cpp
//! \author Kleber Garcia
//! \date   1st september 2014
//! \brief  blockscript AST greedy allocator. All memory gets deallocated at once.

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/BlockScript/AstAllocator.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

//increase the page list 10 elements at a time
const int AstAllocator::sPageIncrement = 10;

AstAllocator::AstAllocator() : 
    mAllocator(nullptr), 
    mMemoryPages(nullptr),
    mMemoryPageListCount(0),
    mMemoryPageListSize(0),
    mMemorySize(0),
    mPageSize(0)
{
}

AstAllocator::~AstAllocator()
{
    FreeMemory();
}

void AstAllocator::Initialize(int pageSize, Alloc::IAllocator* alloc)
{
    mAllocator = alloc;
    mPageSize = pageSize;
}

void* AstAllocator::Alloc(
        size_t tsize, 
        Alloc::Flags flags, 
        Alloc::Category category, 
        const char* debugText, 
        const char* file, 
        unsigned int line
)
{
    int size = static_cast<int>(tsize);
    PG_ASSERTSTR(size < mPageSize, "Cannot allocate size greater than the page size! Memory trashing to follow.");
    int newMemSize = mMemorySize + size;
	int targetPage = newMemSize / mPageSize;
    if (targetPage >= mMemoryPageListCount)
    {
        PG_ASSERT(targetPage == mMemoryPageListCount);
        int newPagesCount = mMemoryPageListCount + sPageIncrement;
        char ** newList = static_cast<char**>(mAllocator->Alloc(static_cast<size_t>(newPagesCount * sizeof(char*)), Alloc::PG_MEM_PERM, -1, "Page Vector", __FILE__, __LINE__));            
        Utils::Memcpy(newList, mMemoryPages, static_cast<unsigned>(mMemoryPageListCount * sizeof(char*)));
        mAllocator->Delete(mMemoryPages);
        mMemoryPages = newList;
        mMemoryPageListCount = newPagesCount;
    }

    if (targetPage >= mMemoryPageListSize)
    {
        PG_ASSERT(mMemoryPageListSize <= mMemoryPageListCount);
        PG_ASSERT(targetPage == mMemoryPageListSize);        
        mMemoryPages[targetPage] = static_cast<char*>(mAllocator->Alloc(static_cast<size_t>(mPageSize), Alloc::PG_MEM_PERM, -1, "Page Vector", __FILE__, __LINE__));
        ++mMemoryPageListSize;
    }

    int prevPage = (mMemorySize / mPageSize);
    int currentOffset = mMemorySize % mPageSize;
    int offset = prevPage != targetPage ? 0 : currentOffset ;
    mMemorySize = newMemSize + (prevPage != targetPage ? mPageSize - currentOffset : 0);
    return mMemoryPages[targetPage] + offset;
}

void* AstAllocator::AllocAlign(
    size_t size, 
    Alloc::Alignment align, 
    Alloc::Flags flags, 
    Alloc::Category category, 
    const char* debugText, 
    const char* file, 
    unsigned int line
)
{
    PG_FAILSTR("Alloc Align not supported in AstAllocator!");
	return nullptr;
}

void AstAllocator::Delete(void* ptr)
{
    PG_FAILSTR("Atomic deletions not allowed in the greedy Ast Allocator!");
}

void AstAllocator::Reset()
{
    mMemorySize = 0;
}

void AstAllocator::FreeMemory()
{
    Reset();
    for (int p = 0; p < mMemoryPageListSize; ++p)
    {
        mAllocator->Delete(mMemoryPages[p]);
    }
    mAllocator->Delete(mMemoryPages);
    mMemoryPages = nullptr;
    mMemoryPageListCount = 0;
}
