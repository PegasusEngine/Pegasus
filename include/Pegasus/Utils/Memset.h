/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memset.h
//! \author Kleber Garcia/Kevin Boulanger
//! \date	5th May 2014
//! \brief	Memset (all its flavors)

namespace Pegasus {
namespace Utils {

//! Fill memory with a constant value in chunks of 8 bits
//! \param destination Memory destination
//! \param value Value to set (8 bits)
//! \param size Total size of destination, in bytes
//! \return Destination buffer
void * Memset8(void * destination, char value, unsigned int size);

//! Fill memory with a constant value in chunks of 32 bits
//! \warning size must be a multiple of 4. If not, the last few bytes will be missing
//! \param destination Memory destination
//! \param value Value to set (32 bits)
//! \param size Total size of destination, in bytes, multiple of 4
//! \return Destination buffer
void * Memset32(void * destination, unsigned long value, unsigned int size);

}   // namespace Utils
}   // namespace Pegasus
