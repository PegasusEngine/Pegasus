/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Io.h
//! \author Kleber Garcia
//! \date   14th October 2013
//! \brief  File loading, shortcuts for IO

#ifndef PEGASUS_IO_H
#define PEGASUS_IO_H

#include "Pegasus/Core/Shared/IoErrors.h"

namespace Pegasus
{

namespace Io
{

class FileBuffer
{
public:
    FileBuffer();
    ~FileBuffer();
    
    void OwnBuffer(char * buffer, int bufferSize);
    void ForgetBuffer();
    void DestroyBuffer();
    void SetFileSize(int fileSize);

    char * GetBuffer() 
    {
        return mBuffer;
    }

    const char * GetBuffer() const
    {
        return mBuffer;
    }
    
    int GetBufferSize() const
    {
        return mBufferSize;
    } 

    int GetFileSize() const
    {
        return mFileSize;
    }

private:
    //no copies allowed
    explicit FileBuffer(const FileBuffer& other); 
    FileBuffer& operator= (const FileBuffer& other);

    char * mBuffer;
    int mFileSize;
    int mBufferSize;
};

// utility function that allocates memory and dumps it into an output buffer.
// the buffer must be deallocated by the caller
IoError OpenFileToBuffer(const char * relativePath, FileBuffer& outputBuffer, bool allocateBuffer);


}

}
#endif
