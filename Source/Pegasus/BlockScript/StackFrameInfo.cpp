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
#include "Pegasus/BlockScript/TypeTable.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

StackFrameInfo::StackFrameInfo()
 : 
mSize(0),
mCreatorCategory(StackFrameInfo::NONE),
mCreatedLine(-1),
mParent(-1)
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
    mParent = -1;
    mEntries.Reset();
}

int StackFrameInfo::Allocate(const char* name, int type, TypeTable& typeTable)
{
    StackFrameInfo::Entry& e = mEntries.PushEmpty();
    PG_ASSERT(Utils::Strlen(name) + 1 < IddStrPool::sCharsPerString);
    Utils::Strcat(e.mName, name);
    int sz = 0;
    bool res = typeTable.ComputeSize(type, sz);
    PG_ASSERT(res);
    e.mOffset = mSize;
    e.mType = type;
    mSize += sz;
    return e.mOffset;
}

bool StackFrameInfo::FindDeclaration(const char* name, int& offset, int& type)
{
    int total = mEntries.Size();
    for (int i = 0; i < total; ++i)
    {
        StackFrameInfo::Entry& e = mEntries[i];
        if (!Utils::Strcmp(name, e.mName))
        {
            offset = e.mOffset;
            type   = e.mType;
            return true;
        }
    }

    return false;
}

void StackFrameInfo::SetDebugInfo(int line, CreatorCategory category)
{
    mCreatedLine = line;    
    mCreatorCategory = category;
}

void StackFrameInfo::GetDebugInfo(int& line, CreatorCategory& category)
{
    line = mCreatedLine;
    mCreatorCategory = category;
}
