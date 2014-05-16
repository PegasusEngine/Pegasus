/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memset.h
//! \author Kleber Garcia	
//! \date	5th May 2014
//! \brief	Memset (all its flavors)

namespace Pegasus
{
namespace Utils
{
    //! Memset8
    //! \brief sets memory in chunks of 8 bytes
    //! \param destination - memory destination
    //! \param value - value to set
    //! \param size - total size of destination, in bytes
    void* Memset8(void * destination, char value, unsigned size);
}
}
