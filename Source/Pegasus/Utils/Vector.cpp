/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Vector.cpp
//! \author Kleber Garcia
//! \date   November 2nd 2014
//! \brief  Pegasus resizable vector. Favoring small code for performance

#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/Memcpy.h"

using namespace Pegasus;
using namespace Pegasus::Utils;

BaseVector::BaseVector(Alloc::IAllocator* alloc, int typeSize)
    : mAlloc(alloc),
      mElementByteSize(typeSize),
      mData(nullptr),
      mDataSize(0),
      mDataCount(0)
{
}

BaseVector::~BaseVector()
{
    Clear();
}

void* BaseVector::PushEmpty()
{
    if (mDataCount <= mDataSize)
    {
        const int PAGE_SIZE = 64;
        void* oldData = mData;
        
        mData = PG_NEW_ARRAY(mAlloc, -1, "Vector Page", Alloc::PG_MEM_PERM, char, (mDataCount + PAGE_SIZE)*mElementByteSize);

        if (oldData != nullptr)
        {
            Utils::Memcpy(mData, oldData, mDataCount*mElementByteSize);
            PG_DELETE_ARRAY(mAlloc,  static_cast<char*>(oldData));
        }

        mDataCount += PAGE_SIZE;
    }

    return static_cast<char*>(mData) + (mDataSize++) * mElementByteSize;
}

void BaseVector::Delete(int index)
{
    PG_ASSERT(index >= 0 && index < mDataSize);
    char* memToDelete = static_cast<char*>(mData) + index * mElementByteSize;
    if (index < mDataSize - 1)
    {
        Utils::Memcpy(memToDelete, memToDelete + mElementByteSize, (mDataSize - index - 1)*mElementByteSize);
    }
    --mDataSize;
}

void BaseVector::Clear()
{
    if (mData)
    {
        PG_DELETE_ARRAY(mAlloc, static_cast<char*>(mData));
    }
    mData = nullptr;
    mDataSize = 0;
    mDataCount = 0;
}
