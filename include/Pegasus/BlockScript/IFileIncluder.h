/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IFileIncluder.h
//! \author Kleber Garcia
//! \date   March 14, 2015
//! \brief  Includer interface. Pass this on a compiler and it will trigger Open callback 
//!         and close callback when a file is included. These should be handled by the 
//          runtime respectfully.

#ifndef PEGASUS_INCLUDER_H
#define PEGASUS_INCLUDER_H

namespace Pegasus
{
namespace BlockScript
{

class IFileIncluder
{
public:
    IFileIncluder() {}
    virtual ~IFileIncluder() {}

    //! Callback, triggered when a file is being included
    //! \param filePath the path of the file to be included
    //! \param outBuffer output parameter, set the output buffer here.
    //! \param outBufferSize output parameter of the size that corresponds to the outBuffer
    //! \return bool true if successful, false otherwise
    virtual bool Open (const char* filePath, const char** outBuffer, int& outBufferSize) = 0;

    // If Open returns true, this function is guaranteed to be executed.
    //! \param buffer - the buffer that was filled in outBuffer in the Open function
    virtual void Close(const char* buffer) = 0;

};

}
}

#endif
