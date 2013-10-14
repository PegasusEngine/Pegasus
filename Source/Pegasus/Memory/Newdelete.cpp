/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	newdelete.cpp
//! \author	Kleber Garcia
//! \date	11th October 2013
//! \brief	Global memory operator overrides

#include <stdlib.h>
#include "Pegasus\Memory\memory.h"
#include "Pegasus/Core/Log.h"

void * operator new (size_t size)
{
    //DANGER! pegasus naked operator used. Forcing a crash, please refer to the pegasus memory framework
    //and use one of the predesposed macros.
    return 0;
}

void * operator new(size_t size, void * allocator,  Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line)
{
    // Todo, add allocator implementation
    return malloc(size);
}

void * operator new[] (size_t size, void * allocator,  Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line)
{
    // Todo, add allocator implementation
    return malloc(size);
}

void operator delete(void* pointer, const char * file, int line)
{
    free(pointer);
}

void operator delete[](void* pointer, const char * file, int line)
{
    free(pointer);
}
