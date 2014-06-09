/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memset.cpp
//! \author Kleber Garcia	
//! \date	5th May 2014
//! \brief	Memset implementation (all its flavors)

namespace Pegasus
{
namespace Utils
{

void* Memset8(void * destination, char value, unsigned int size)
{
    char * byteDestination = static_cast<char*>(destination);
    while(size-- > 0)
    {
        *byteDestination++ = value;
    }
    return destination;
}


}
}
