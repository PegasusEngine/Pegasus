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
   ERR_NONE, //!< No error
   ERR_FILE_NOT_FOUND,  //!< No file could be open
   ERR_BUFFER_TOO_SMALL, //!< The buffer passed is too small
   ERR_FILE_SIZE_TOO_BIG, //!< The file size is > than 32bit
   ERR_READING_FILE, //!< An error occured while reading the file
   ERR_OPENING_FILE, //!< An error occured while attempting to open a file
   ERR_WRITING_FILE  //!< An error during the write function
};

} // namespace Io
} // namespace Pegasus

#endif
