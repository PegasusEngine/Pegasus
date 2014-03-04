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

#define PG_ASSERTSTR_TEST(x, msg) if(!(x)) {*reinterpret_cast<char*>(0x0) = 0x0;}

#if   PEGASUS_POINTERSIZE_64BIT 
typedef unsigned long long NumPtr;
#elif PEGASUS_POINTERSIZE_32BIT 
typedef  int unsigned NumPtr;
#endif

//! Memcpy
void * Pegasus::Utils::Memcpy(void* destination, const void* source, unsigned count)
{
    PG_ASSERTSTR_TEST( 
        reinterpret_cast<NumPtr>(destination) < reinterpret_cast<NumPtr>(source) ?
            reinterpret_cast<NumPtr>(source) - reinterpret_cast<NumPtr>(destination) > static_cast<NumPtr>(count) :
            reinterpret_cast<NumPtr>(destination) - reinterpret_cast<NumPtr>(source) > static_cast<NumPtr>(count) ,
        "Fatal Memcpy!, memcpy intersection detected. Pegasus only supports fwd copy. this will result in a possible memory stomp."
    ); 
    int blockSize = sizeof(NumPtr); //platform independent initial type size
    int blockCount = count / blockSize; 
    char * dst = static_cast<char*>(destination);
    const char * src = static_cast<const char*>(source);
    while (count > 0)
    {
        if (blockCount == 0) 
        {
            blockSize >>= 1;
            blockCount = count / blockSize;
        }
        else
        {
            switch(blockSize)
            {
            case sizeof(char):
                *(reinterpret_cast<char*>(dst)) = *(reinterpret_cast<const char*>(src));
                break;
            case sizeof(short):
                *(reinterpret_cast<short*>(dst)) = *(reinterpret_cast<const short*>(src));
                break;
            case sizeof(int):
                *(reinterpret_cast<int*>(dst)) = *(reinterpret_cast<const int*>(src));
                break;
            case sizeof(long long):
                *(reinterpret_cast<long long*>(dst)) = *(reinterpret_cast<const long long*>(src));
                break;
            default:
                ;
                //nop
            }
            dst += blockSize;
            src += blockSize;
            count -= blockSize;
            --blockCount;
        }
    }
    return destination;
    
}
