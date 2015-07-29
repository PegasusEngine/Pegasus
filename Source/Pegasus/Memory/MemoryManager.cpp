/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MemoryManager.cpp
//! \author David Worsham
//! \date   08th November 2013
//! \brief  Memory manager, to manage a set of allocators for an application.

#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Memory/MallocFreeAllocator.h"

namespace Pegasus {
namespace Memory {

// Global allocator
//! \todo Real allocator / heap management...
static MallocFreeAllocator sGlobalAllocator(0);
static MallocFreeAllocator sCoreAllocator(1);
static MallocFreeAllocator sRenderAllocator(2);
static MallocFreeAllocator sNodeAllocator(3);
static MallocFreeAllocator sNodeDataAllocator(4);
static MallocFreeAllocator sPropertyPointerAllocator(5);
static MallocFreeAllocator sTimelineAllocator(6);
static MallocFreeAllocator sWindowAllocator(7);

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetGlobalAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetCoreAllocator()
{
    return &sCoreAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetRenderAllocator()
{
    return &sRenderAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetNodeAllocator()
{
    return &sNodeAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetNodeDataAllocator()
{
    return &sNodeDataAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetPropertyPointerAllocator()
{
    return &sPropertyPointerAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetTimelineAllocator()
{
    return &sTimelineAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetWindowAllocator()
{
    return &sWindowAllocator;
}


}   // namespace SubProjectNamespace
}   // namespace Pegasus
