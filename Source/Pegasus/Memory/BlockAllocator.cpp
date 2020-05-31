/****************************************************************************************/
/*                                                                                      */
/*                                       pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockAllocator.cpp
//! \author Kleber Garcia
//! \date   8th February 2015
//! \brief  BlockAllocator greedy allocator. All memory gets deallocated at once.

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Memory/BlockAllocator.h"

using namespace Pegasus;
using namespace Pegasus::Memory;

//increase the page list 10 elements at a time
const int BlockAllocator::sPageIncrement = 10;

BlockAllocator::BlockAllocator() : 
    mAllocator(nullptr), 
    mMemoryPages(nullptr),
    mMemoryPageListCount(0),
    mMemoryPageListSize(0),
    mMemorySize(0),
    mPageSize(0)
{
}

BlockAllocator::~BlockAllocator()
{
    FreeMemory();
}

void BlockAllocator::Initialize(int pageSize, Alloc::IAllocator* alloc)
{
    mAllocator = alloc;
    mPageSize = pageSize;
}

void* BlockAllocator::Alloc(
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
    size_t newMemSize = mMemorySize + size;
	size_t targetPage = newMemSize / mPageSize;
    if (targetPage >= mMemoryPageListCount)
    {
        PG_ASSERT(targetPage == mMemoryPageListCount);
        int newPagesCount = mMemoryPageListCount + sPageIncrement;
        char ** newList = static_cast<char**>(mAllocator->Alloc(static_cast<size_t>(newPagesCount * sizeof(char*)), Alloc::PG_MEM_PERM, -1, "Page Vector", __FILE__, __LINE__));            
		if (mMemoryPages != nullptr)
		{
			Utils::Memcpy(newList, mMemoryPages, static_cast<unsigned>(mMemoryPageListCount * sizeof(char*)));
			mAllocator->Delete(mMemoryPages);
		}
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

	
    size_t prevPage = (mMemorySize / mPageSize);
	bool isUnaligned = (((mMemorySize + size) % mPageSize) != 0) && (prevPage != targetPage);
    size_t currentOffset = mMemorySize % mPageSize;
    size_t offset = isUnaligned ? 0 : currentOffset ;
	size_t actualPage = isUnaligned ? targetPage : prevPage;
    mMemorySize = newMemSize + (isUnaligned ? mPageSize - currentOffset : 0);
    return mMemoryPages[actualPage] + offset;
}

void* BlockAllocator::AllocAlign(
    size_t size, 
    Alloc::Alignment align, 
    Alloc::Flags flags, 
    Alloc::Category category, 
    const char* debugText, 
    const char* file, 
    unsigned int line
)
{
    PG_FAILSTR("Alloc Align not supported in BlockAllocator!");
	return nullptr;
}

void BlockAllocator::Delete(void* ptr)
{
    PG_FAILSTR("Atomic deletions not allowed in the greedy Ast Allocator!");
}

void BlockAllocator::Reset()
{
    mMemorySize = 0;
}

void BlockAllocator::FreeMemory()
{
    Reset();
    for (int p = 0; p < mMemoryPageListSize; ++p)
    {
        mAllocator->Delete(mMemoryPages[p]);
    }
    if (mMemoryPages != nullptr)
    {
        mAllocator->Delete(mMemoryPages);
	}
    mMemoryPages = nullptr;
    mMemoryPageListCount = 0;
    mMemoryPageListSize = 0;
}
