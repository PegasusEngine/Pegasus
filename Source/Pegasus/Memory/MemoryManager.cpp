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

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetGlobalAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetCoreAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetRenderAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetNodeAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetNodeDataAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

Alloc::IAllocator* GetWindowAllocator()
{
    return &sGlobalAllocator;
}


}   // namespace SubProjectNamespace
}   // namespace Pegasus
