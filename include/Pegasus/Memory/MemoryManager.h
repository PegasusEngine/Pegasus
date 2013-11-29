/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   MemoryManager.h
//! \author David Worsham
//! \date   08th November 2013
//! \brief  Memory manager, to manage a set of allocators for an application.

#ifndef PEGASUS_MEMORY_MEMORYMANAGER_H
#define PEGASUS_MEMORY_MEMORYMANAGER_H

#include "Pegasus/Allocator/IAllocator.h"

namespace Pegasus {
namespace Memory {


//! Get the global allocator
//! \return Global allocator, for the global heap
Alloc::IAllocator* GetGlobalAllocator();

//! Get the core allocator
//! \return Core allocator
Alloc::IAllocator* GetCoreAllocator();

//! Get the render allocator
//! \return Render allocator
Alloc::IAllocator* GetRenderAllocator();

//! Get the node allocator (for all node memory, except the attached NodeData)
//! \return Node allocator
Alloc::IAllocator* GetNodeAllocator();

//! Get the node data allocator (only for NodeData)
//! \return Node data allocator
Alloc::IAllocator* GetNodeDataAllocator();

//! Get the timeline allocator (for the timeline, lanes and blocks)
//! \return Timeline allocator
Alloc::IAllocator* GetTimelineAllocator();

//! Get the window allocator
//! \return Window allocator
Alloc::IAllocator* GetWindowAllocator();


}   // namespace Memory
}   // namespace Pegasus

#endif // PEGASUS_MEMORY_MEMORYMANAGER_H
