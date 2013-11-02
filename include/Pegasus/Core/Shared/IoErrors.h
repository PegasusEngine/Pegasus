/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IoErrors.h
//! \author Kleber Garcia
//! \date   21st October 2013
//! \brief  File loading errors, exposed to the world

#ifndef PEGASUS_IOERRORS_H
#define PEGASUS_IOERRORS_H

namespace Pegasus {
namespace Io {

//! Error code return for IO operations
enum IoError
{
   ERR_NONE, // no error
   ERR_FILE_NOT_FOUND,  //no file could be open
   ERR_BUFFER_TOO_SMALL, // the buffer passed is too small
   ERR_FILE_SIZE_TOO_BIG, // the file size is > than 32bit
   ERR_READING_FILE // an error occured while reading the file
};

} // namespace Io
} // namespace Pegasus

#endif