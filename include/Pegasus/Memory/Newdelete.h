/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	newdelete.h
//! \author	Kleber Garcia
//! \date	11th October 2013
//! \brief	Global memory operator overrides

#ifndef PEGASUS_NEWDELETE_H
#define PEGASUS_NEWDELETE_H

namespace Pegasus
{

namespace Memory
{
    enum Flags
    {
        PG_MEM_TEMP, //temporal memory
        PG_MEM_PERM  //permanent memory
    };
}

}
void * operator new (size_t size);
void * operator new(size_t size, void * allocator, Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line);
void * operator new[] (size_t size, void * allocator, Pegasus::Memory::Flags flags, const char * debugText, const char * file, int line);

void operator delete(void* pointer, const char * file, int line);
void operator delete[](void* pointer, const char * file, int line);

#endif
