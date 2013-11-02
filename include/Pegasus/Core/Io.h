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

namespace Pegasus {
namespace Io {

//! Buffer for a loaded file.  The file data may reside in a buffer owned by
//! this object, or one owned by a different subsystem.
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


//! Config structure for the IO manager
struct IOManagerConfig
{
public:
    //! Constructor
    IOManagerConfig() : mBasePath(nullptr), mAppName(nullptr) {}
    //! Destructor
    ~IOManagerConfig() {}


    const char* mBasePath; //<! The base path to load all assets from
    const char* mAppName; //<! Name of the app containing this manager, for building the path
};


//! IO manager, loads files/assets from a given root filesystem
class IOManager
{
public:
    //! Constructor
    //! \param Configuration structure for this IOManager.
    IOManager(const IOManagerConfig& config);
    //! Destructor
    ~IOManager();


    //! Gets the root filesystem for this manager
    //! \return Root filesystem path.
    inline const char* GetRoot() const { return mRootDirectory; }


    //! Utility function that allocates memory and dumps it into an output buffer
    //! \param relativePath Relative path to the file, within the asset root.
    //! \param outputBuffer Output buffer, in which the loaded file is stored.
    //! \param allocateBuffer Whether to allocate the buffer inside of outputBuffer, or use a pre-allocated one.  This is a performance optimization.
    //! \return Error code.
    //! \note Buffer must be deallocated by the caller
    IoError OpenFileToBuffer(const char* relativePath, FileBuffer& outputBuffer, bool allocateBuffer);


    static const unsigned int MAX_FILEPATH_LENGTH = 256; //<! Max length for a file path

private:
    // No copies allowed
    PG_DISABLE_COPY(IOManager);

    char mRootDirectory[MAX_FILEPATH_LENGTH]; //<! Root directory this manager loads files from
};


} // namespace Io
} // namespace Pegasus

#endif
