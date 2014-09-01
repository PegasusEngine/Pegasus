/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IddStrPool.cpp
//! \author Kleber Garcia
//! \date   31th August 2014
//! \brief  String pool allocator class implementation

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/IddStrPool.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Memory/MemoryManager.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

IddStrPool::IddStrPool()
: mAllocator(nullptr),
  mStringCount(0)
{
}

IddStrPool::~IddStrPool()
{
    Clear();
}

void IddStrPool::Initialize(Alloc::IAllocator* allocator)
{
    PG_ASSERT(mStringCount == 0);
    mAllocator = allocator;
}

void IddStrPool::Clear()
{
	int pageSize = GetPageCount();
    for (int i = 0; i < pageSize; ++i)
    {
        mAllocator->Delete(mPages[i]);
    }
    mStringCount = 0;
}

//lazily allocate a page (a set of strings) when required.
char* IddStrPool::AllocateString()
{
    if (GetStringCount() < sMaxStrings)
    {
        int newStringCount = mStringCount + 1;
        int targetPage = newStringCount / sMaxStringsPerPages;

        if (targetPage >= GetPageCount())
        {
            PG_ASSERT(targetPage == GetPageCount());
            AllocatePage();    
        }
        
        char* mem = mPages[targetPage] + ((mStringCount*sCharsPerString) % sPageByteSize);
        mStringCount = newStringCount;
        return mem;
    }
    else
    {
        return nullptr;
    }
}

void IddStrPool::AllocatePage()
{
    if (GetPageCount() < sMaxPages)
    {
        int newPage = GetPageCount();
        mPages[newPage] = static_cast<char*>(mAllocator->Alloc(sPageByteSize, Alloc::PG_MEM_TEMP, -1, "IddStringPool::mPage", __FILE__, __LINE__));
    }
    else
    {
        PG_FAILSTR("Attempting to allocate too many pages for strings.");
    }
}
