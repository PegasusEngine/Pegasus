/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Memory.cpp
//! \author David Worsham
//! \date   02 Nov 2013
//! \brief  Memory framework

#include "Pegasus/Memory/Memory.h"
#include "Pegasus/Memory/MallocFreeAllocator.h"

namespace Pegasus {
namespace Memory {

// Global allocator
//! \todo Real allocator / heap management...
static MallocFreeAllocator sGlobalAllocator;

//----------------------------------------------------------------------------------------

IAllocator* GetGlobalAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

IAllocator* GetCoreAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

IAllocator* GetRenderAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

IAllocator* GetNodeAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

IAllocator* GetNodeDataAllocator()
{
    return &sGlobalAllocator;
}

//----------------------------------------------------------------------------------------

IAllocator* GetWindowAllocator()
{
    return &sGlobalAllocator;
}


}   // namespace SubProjectNamespace
}   // namespace Pegasus
