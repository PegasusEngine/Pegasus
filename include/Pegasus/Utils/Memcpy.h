/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Memcpy.h
//! \author	Kleber Garcia
//! \date	11th January 2014
//! \brief	Memcpy fast / dirty implementation

#ifndef PEGASUS_STDC_MEMCPY_H
#define PEGASUS_STDC_MEMCPY_H

namespace Pegasus
{
namespace Utils
{

//! Standard STD C based lite memcpy function
//! \brief Does not support intersecting memory like the actual std function does
//!        it also does not do memory alignment check, since recent processors handle this on
//!        the medal.
void * Memcpy(void* destination, const void* source, unsigned count);

}
}

#endif
