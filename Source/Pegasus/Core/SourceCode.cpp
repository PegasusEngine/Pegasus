#include "Pegasus/Core/SourceCode.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Allocator/Alloc.h"

using namespace Pegasus;
using namespace Pegasus::Core;

SourceCode::SourceCode(Alloc::IAllocator* allocator)
: mAllocator(allocator), mParents(allocator), mLockParentArray(false)
{
}

SourceCode::~SourceCode()
{
    ClearParents();
}


void SourceCode::SetSource(const char * src, int srcSize)
{
    InvalidateData();
    //reallocate buffer size if more space requested on recompilation
    if (srcSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer (
            mAllocator,
            PG_NEW_ARRAY(mAllocator, -1, "shader src", Pegasus::Alloc::PG_MEM_PERM, char, srcSize),
            srcSize
        );
    }
    mFileBuffer.SetFileSize(srcSize);
    PG_ASSERTSTR(mFileBuffer.GetBufferSize() >= srcSize, "Not enough size to hold the string buffer!");
    Pegasus::Utils::Memcpy(mFileBuffer.GetBuffer(),src,srcSize);
}


void SourceCode::GetSource (const char ** outSrc, int& outSize) const
{
    *outSrc = mFileBuffer.GetBuffer(); 
    outSize = mFileBuffer.GetFileSize();
}


void SourceCode::Compile()
{
    //copy this vector to a temp vector to avoid pop / push issues

    mLockParentArray = true;
    for (int i = 0; i < mParents.GetSize(); ++i)
    {
        mParents[i]->InvalidateData();
        mParents[i]->Compile();
    }
    mLockParentArray = false;
}


void SourceCode::RegisterParent(SourceCode* parent)
{
    if (!mLockParentArray)
    {
        mParents.PushEmpty() = parent;
    }
}


void SourceCode::UnregisterParent(SourceCode* parent)
{
    if (!mLockParentArray)
    {
        for (int i = 0; i < mParents.GetSize(); ++i)
        {
            if (mParents[i] == parent)
            {
                mParents.Delete(i);
                return;
            }
        }
        PG_FAILSTR("Trying to unregister a source file that has no parent?");
    }

}

void SourceCode::ClearParents()
{
    if (!mLockParentArray)
    {
        mParents.Clear();
    }
}


