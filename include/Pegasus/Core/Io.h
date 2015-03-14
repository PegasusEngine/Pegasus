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

// forward declarations
namespace Pegasus {
    namespace Alloc {
        class IAllocator;
    }
}

//----------------------------------------------------------------------------------------

namespace Pegasus {
namespace Io {

//! Buffer for a loaded file.  The file data may reside in a buffer owned by
//! this object, or one owned by a different subsystem.
class FileBuffer
{
public:
    //! Constructor
    FileBuffer();

    //! Destructor
    ~FileBuffer();


    //! Takes ownership of a buffer, as the contents of this object
    //! \param bufferAlloc Allocator used to create this buffer.
    //! \param buffer Address of the buffer.
    //! \param bufferSize Size of the buffer.
    void OwnBuffer(Alloc::IAllocator* bufferAlloc, char * buffer, int bufferSize);

    //! Releases ownership of any currently owned buffer
    void ForgetBuffer();

    //! Destroys the currently owned buffer
    void DestroyBuffer();


    //! Sets the size of the contained file
    //! \param fileSize New file size.
    //! \warning File size must be less than the container/buffer size
    void SetFileSize(int fileSize);

    //! Gets the contained buffer, allowing RW operations
    //! \return The buffer.
    inline char* GetBuffer() { return mBuffer; }

    //! Gets the contained buffer
    //! \return The buffer.
    const char * GetBuffer() const { return mBuffer; }
    
    //! Gets the size of the contained buffer
    //! \return Buffer size.
    int GetBufferSize() const { return mBufferSize; }

    //! Gets the size of the contained file
    //! \return Filesize.
    int GetFileSize() const { return mFileSize; }

    //! Returns allocator
    //! \return allocator
    Alloc::IAllocator* GetAllocator() const { return mAllocator; }

private:

    Alloc::IAllocator* mAllocator; //!< Allocator used to create the buffer
    char* mBuffer; //!< Contained buffer
    int mFileSize; //!< Size of the file in the buiffer
    int mBufferSize; //!< Size of the buffer
};

//----------------------------------------------------------------------------------------

//! IO manager, loads files/assets from a given root filesystem
class IOManager
{
public:
    //! Constructor
    //! \param Configuration structure for this IOManager.
    explicit IOManager(const char* rootPath);

    //! Destructor
    ~IOManager();


    //! Gets the root filesystem for this manager
    //! \return Root filesystem path.
    inline const char* GetRoot() const { return mRootDirectory; }


    //! Utility function that allocates memory and dumps it into an output buffer
    //! \param relativePath Relative path to the file, within the asset root.
    //! \param outputBuffer Output buffer, in which the loaded file is stored.
    //! \param allocateBuffer Whether to allocate the buffer inside of outputBuffer, or use a pre-allocated one.  This is a performance optimization.
    //! \param alloc Allocator to use when allocating the buffer.
    //! \return Error code.
    //! \note Buffer must be deallocated by the caller
    IoError OpenFileToBuffer(const char* relativePath, FileBuffer& outputBuffer, bool allocateBuffer = false, Alloc::IAllocator* alloc = nullptr);

    //! Utility function that writes binary data to a file
    //! \param relativePath Relative path to the file, within the asset root.
    //! \param inputBuffer the file buffer to dump into the file.
    //! \return Error code.
    IoError SaveFileToBuffer(const char* relativePath, const FileBuffer& inputBuffer);


    static const unsigned int MAX_FILEPATH_LENGTH = 256; //!< Max length for a file path

private:
    // No copies allowed
    PG_DISABLE_COPY(IOManager);


    char mRootDirectory[MAX_FILEPATH_LENGTH]; //!< Root directory this manager loads files from
};


} // namespace Io
} // namespace Pegasus

#endif
