/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   NewDelete.cpp
//! \author Kleber Garcia
//! \date   11th October 2013
//! \brief  Global memory operator overrides

#include "Pegasus/Memory/NewDelete.h"
#include "Pegasus/Memory/Memory.h"

void* operator new(size_t size)
{
    return Pegasus::Memory::GetGlobalAllocator()->Alloc(size, Pegasus::Memory::PG_MEM_PERM, nullptr, __FILE__, __LINE__);
}

//----------------------------------------------------------------------------------------

void* operator new[](size_t size)
{
    return Pegasus::Memory::GetGlobalAllocator()->AllocArray(size, Pegasus::Memory::PG_MEM_PERM, nullptr, __FILE__, __LINE__);
}

//----------------------------------------------------------------------------------------

void* operator new(size_t size, Pegasus::Memory::IAllocator* allocator, Pegasus::Memory::Flags flags, const char* debugText, const char* file, int line)
{
    return allocator->Alloc(size, flags, debugText, file, line);
}

//----------------------------------------------------------------------------------------

void* operator new[](size_t size, Pegasus::Memory::IAllocator* allocator, Pegasus::Memory::Flags flags, const char* debugText, const char* file, int line)
{
    return allocator->AllocArray(size, flags, debugText, file, line);
}

//----------------------------------------------------------------------------------------

void operator delete(void* pointer)
{
    Pegasus::Memory::GetGlobalAllocator()->Delete(pointer, __FILE__, __LINE__);
}

//----------------------------------------------------------------------------------------

void operator delete[](void* pointer)
{
    Pegasus::Memory::GetGlobalAllocator()->DeleteArray(pointer, __FILE__, __LINE__);
}

