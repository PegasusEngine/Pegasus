/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ByteStream.cpp
//! \author Kleber Garcia
//! \date   2/10/2015
//! \brief  Pegasus resizable byte stream. Use this class to add elements to a constant growing
//!         vector

#include "Pegasus/Utils/ByteStream.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Utils/Memcpy.h"


using namespace Pegasus;
using namespace Pegasus::Utils;

ByteStream::ByteStream(Alloc::IAllocator* allocator)
    : mAllocator(allocator),
      mBuffer(nullptr),
      mBufferSize(0),
      mTotalCount(0)
{
    
}

ByteStream::~ByteStream()
{
    Reset();
}

void ByteStream::Append(const void* buffer, int size)
{
    if (mBufferSize + size > mTotalCount)
    {
        mTotalCount = 2 * (mBufferSize + size);
        void* newBuffer = PG_NEW_ARRAY(mAllocator, -1, "ByteStream", Alloc::PG_MEM_TEMP, char, mTotalCount);
        if (mBuffer != nullptr)
        {
            Utils::Memcpy(newBuffer, mBuffer, mBufferSize);
            PG_DELETE_ARRAY(mAllocator, static_cast<char*>(mBuffer));
        }
        mBuffer = newBuffer;
    }

    Utils::Memcpy(static_cast<char*>(mBuffer) + mBufferSize, buffer, size);
    mBufferSize += size;
    
}

void ByteStream::Append(const ByteStream* byteStream)
{
    Append(byteStream->GetBuffer(), byteStream->GetSize());
}

void ByteStream::Reset()
{
    if (mBuffer != nullptr)
    {
        PG_DELETE_ARRAY(mAllocator, static_cast<char*>(mBuffer));
        mBuffer = nullptr;
        mTotalCount = 0;
        mBufferSize = 0;
    }
}
