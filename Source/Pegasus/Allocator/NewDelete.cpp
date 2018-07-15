/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   NewDelete.cpp
//! \author Kleber Garcia
//! \date   11th October 2013
//! \brief  Global memory operator overrides

#include "Pegasus/Allocator/internal/NewDelete.h"

// Common method to force a crash via nullptr dereference
static inline void ForceCrash()
{
    volatile char* crashMe = nullptr;

    *crashMe = 0;
}

//----------------------------------------------------------------------------------------
/*
void* operator new(size_t size)
{
    // Force crash
    // Don't use this, use PG_NEW instead
    ForceCrash();

    return nullptr;
}

//----------------------------------------------------------------------------------------

void* operator new[](size_t size)
{
    // Force crash
    // Don't use this, use PG_NEW_ARRAY instead
    ForceCrash();

    return nullptr;
}
*/
//----------------------------------------------------------------------------------------

// PG_NEW
void* operator new(size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char* debugText, const char* file, int line)
{
    return allocator->Alloc(size, flags, cat, debugText, file, line);
}

//----------------------------------------------------------------------------------------

// PG_NEW_ALIGN
void* operator new(size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char* debugText, const char* file, int line)
{
    return allocator->AllocAlign(size, align, flags, cat, debugText, file, line);
}

//----------------------------------------------------------------------------------------

void* operator new[](size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char* debugText, const char* file, int line)
{
    // Force crash
    // Don't use this, use PG_NEW_ARRAY instead
    ForceCrash();

    return nullptr;
}

//----------------------------------------------------------------------------------------

void* operator new[](size_t size, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char* debugText, const char* file, int line)
{
    // Force crash
    // Don't use this, use PG_NEW_ARRAY_ALIGN instead
    ForceCrash();

    return nullptr;
}

//----------------------------------------------------------------------------------------
/*
void operator delete(void* pointer)
{
    // Force crash
    // Don't use this, use PG_DELETE instead
    ForceCrash();
}
*/

//----------------------------------------------------------------------------------------

void operator delete(void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line)
{
    // Force crash
    // Don't use this, use PG_DELETE instead
    ForceCrash();
}

//----------------------------------------------------------------------------------------

void operator delete(void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line)
{
    // Force crash
    // Don't use this, use PG_DELETE instead
    ForceCrash();
}

//----------------------------------------------------------------------------------------

void operator delete[](void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line)
{
    // Force crash
    // Don't use this, use PG_DELETE_ARRAY instead
    ForceCrash();
}

//----------------------------------------------------------------------------------------

void operator delete[](void* pointer, Pegasus::Alloc::IAllocator* allocator, Pegasus::Alloc::Alignment align, Pegasus::Alloc::Flags flags, Pegasus::Alloc::Category cat, const char * debugText, const char * file, int line)
{
    // Force crash
    // Don't use this, use PG_DELETE_ARRAY instead
    ForceCrash();
}

