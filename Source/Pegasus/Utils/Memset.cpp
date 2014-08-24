/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memset.cpp
//! \author Kleber Garcia/Kevin Boulanger
//! \date	5th May 2014
//! \brief	Memset implementation (all its flavors)

#include "Pegasus/Utils/Memset.h"

namespace Pegasus {
namespace Utils {


void* Memset8(void * destination, char value, unsigned int size)
{
    // Copy the major part of the buffer using 32 bit blocks
    if (size >= 4)
    {
        unsigned long value32 = static_cast<unsigned long>(value);
        value32 = (value32 << 8) | value32;
        value32 = (value32 << 16) | value32;
        Memset32(destination, value32, size & 0xFFFFFFFC);
    }

    // Copy the leftover bytes if the buffer size is not a multiple of 4
    if (size & 3)
    {
        char * byteDestination = static_cast<char *>(destination);
        byteDestination += size & 0xFFFFFFFC;
        size = size & 3;
        while (size-- > 0)
        {
            *byteDestination++ = value;
        }
    }

    return destination;
}

//----------------------------------------------------------------------------------------

void * Memset32(void * destination, unsigned long value, unsigned int size)
{
    PG_ASSERTSTR((size & 0x3) == 0, "The size of the output buffer must be a multiple of 4");
    unsigned int numBlocks = size >> 2;

    unsigned long * ulongDestination = static_cast<unsigned long *>(destination);
    while (numBlocks-- > 0)
    {
        *ulongDestination++ = value;
    }
    return destination;
}


}   // namespace Utils
}   // namespace Pegasus
