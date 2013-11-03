/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Memory.h
//! \author Kleber Garcia
//! \date   14th October 2013
//! \brief  Memory framework

#ifndef PEGASUS_MEMORY_MEMORY_H
#define PEGASUS_MEMORY_MEMORY_H

#include "Pegasus/Memory/newdelete.h"


//! Macro for allocating memory
#define PG_NEW(debug_str, flags) new(nullptr/*null allocator for now*/, flags, debug_str, __FILE__, __LINE__)

//! Macro for allocating memory as an array
#define PG_NEW_ARRAY(debug_str, flags, type, numElements) new(nullptr/*null allocator for now*/, flags, debug_str, __FILE__, __LINE__) type[numElements]

//! Macro for freeing memory (to use with PG_NEW)
#define PG_DELETE delete

//! Macro for freeing arrays of memory (to use with PG_NEW_ARRAY)
#define PG_DELETE_ARRAY delete[]


#endif // PEGASUS_MEMORY_MEMORY_H
