/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RefCounted.cpp
//! \author Kleber Garcia
//! \date   August 2nd 2015
//! \brief  Refcounted object (basic refcount operations and state tracking) 

#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Allocator/Alloc.h"

using namespace Pegasus;
using namespace Core;

RefCounted::RefCounted(Alloc::IAllocator* allocator)
: mRefCount(0), mAllocator(allocator)
{
    PG_ASSERT(allocator != nullptr);
}

RefCounted::~RefCounted()
{
    PG_ASSERTSTR(mRefCount == 0, "Trying to destroy a Node that still has owners (mRefCount == %d)", mRefCount);
}

void RefCounted::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        PG_DELETE(mAllocator, this);
    }
}
