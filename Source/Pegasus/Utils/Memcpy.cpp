/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memcpy.cpp
//! \author Kleber Garcia	
//! \date	11th January 2014
//! \brief	Memcpy implementation

#include "Pegasus/Utils/Memcpy.h"

#if   PEGASUS_POINTERSIZE_64BIT 
    typedef unsigned long long NumPtr;
#else
    typedef int NumPtr;
#endif

//! Memcpy
void * Pegasus::Utils::Memcpy(void* dst, const void* src, unsigned count)
{
    PG_ASSERTSTR( 
        reinterpret_cast<NumPtr>(dst) < reinterpret_cast<NumPtr>(src) ||
        (reinterpret_cast<NumPtr>(dst) > reinterpret_cast<NumPtr>(src) && (reinterpret_cast<NumPtr>(dst) - reinterpret_cast<NumPtr>(src)) >= static_cast<NumPtr>(count)),
        "Fatal Memcpy!, memcpy intersection detected. Pegasus only supports fwd copy. this will result in a possible memory stomp."
    );

    unsigned blockSize = 0;
    unsigned i = 0;
#if  PEGASUS_POINTERSIZE_64BIT 
    long long * dst64bit = static_cast<long long*>(dst);
    const long long * src64bit = static_cast<const long long*>(src);
    blockSize = (count >> 3); 
    for (i = 0; i < blockSize; ++i)
    {
        *(dst64bit++) = *(src64bit++);
    }
    count -= blockSize << 3;
    dst = dst64bit;
    src = src64bit;
#endif

    int * dst32bit = reinterpret_cast<int*>(dst);
    const int * src32bit = reinterpret_cast<const int*>(src);
    blockSize = (count >> 2);
    for (i = 0; i < blockSize; ++i)
    {
        *(dst32bit++) = *(src32bit++);
    }
    count -= blockSize << 2;

    short * dst16bit = reinterpret_cast<short *>(dst32bit);
    const short * src16bit = reinterpret_cast<const short*>(src32bit);
    blockSize = (count >> 1);
    for (i = 0; i < blockSize; ++i)
    {
        *(dst16bit++) = *(src16bit++);
    }
    count -= blockSize << 1;
    
    char * dst8bit = reinterpret_cast<char*>(dst16bit);
    const char * src8bit = reinterpret_cast<const char*>(src16bit);
    for (i = 0; i < count; ++i)
    {
        *(dst8bit++) = *(src8bit++);
    }

    return dst;
    
}
