/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   StackFrameInfo.cpp
//! \author Kleber Garcia
//! \date   September 8th 2014
//! \brief  Blockscript stack frame information, to be used in order to obtain stack elements (variables)
//!         positions. Implementation.

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/BlockScript/StackFrameInfo.h"
#include "Pegasus/BlockScript/TypeDesc.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

StackFrameInfo::StackFrameInfo()
 : 
mSize(0),
mTempSize(0),
mCreatorCategory(StackFrameInfo::NONE),
mParent(nullptr)
{
}

//! Destructor
StackFrameInfo::~StackFrameInfo()
{
}

void StackFrameInfo::Initialize(Alloc::IAllocator* allocator)
{
    mEntries.Initialize(allocator);
}

void StackFrameInfo::Reset()
{
    mParent = nullptr;
    mEntries.Reset();
}

int StackFrameInfo::Allocate(const char* name, const TypeDesc* type, bool isFunArg)
{
    StackFrameInfo::Entry& e = mEntries.PushEmpty();
    PG_ASSERT(Utils::Strlen(name) + 1 < IddStrPool::sCharsPerString);
    Utils::Strcat(e.mName, name);
    int sz = type->GetByteSize();    
    e.mOffset = mSize;
    e.mType = type;
    e.mIsArg = isFunArg;
    mSize += sz;
    return e.mOffset;
}

int StackFrameInfo::AllocateTemporal(int newSize)
{
    int targetOffset = mTempSize;
    mTempSize += newSize;
    return targetOffset;
}

StackFrameInfo::Entry* StackFrameInfo::FindDeclaration(const char* name)
{
    int total = mEntries.Size();
    for (int i = 0; i < total; ++i)
    {
        StackFrameInfo::Entry& e = mEntries[i];
        if (!Utils::Strcmp(name, e.mName))
        {
            return &e;
        }
    }

    return nullptr;
}
