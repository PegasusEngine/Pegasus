

/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Io.cpp
//! \author Kleber Garcia
//! \date   14th October 2013
//! \brief  File loading, shortcuts for IO

#include "Pegasus/Core/Io.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Utils/String.h"
#include "stdio.h" //using the windows libraries to produce file IO
#if PEGASUS_USE_NATIVE_IO_CALLS
#if PEGASUS_PLATFORM_WINDOWS
#include <windows.h>
#endif
#endif

namespace Pegasus {
namespace Io {

//implementation using native windows api calls
#if PEGASUS_USE_NATIVE_IO_CALLS
namespace internal
{
#if PEGASUS_PLATFORM_WINDOWS

Pegasus::Io::IoError NativeOpenFileToBuffer(const char* path, Pegasus::Io::FileBuffer& outputBuffer, bool allocateBuffer, Alloc::IAllocator* alloc)
{
    // Load the file
    HANDLE fileHandle = CreateFile(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL, //win32 security attributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL //offset structures
    );
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        fileSize.LowPart = 0;
        fileSize.HighPart = 0;
        GetFileSizeEx(fileHandle, &fileSize);
        if (fileSize.HighPart != 0)
        {
            //pegasus does not support files that are bigger than 4 gb!
            PG_FAILSTR("Pegasus does not support files bigger than 4 gb!");
            CloseHandle(fileHandle);
            return Pegasus::Io::ERR_FILE_SIZE_TOO_BIG;
        }
        else
        {
            if (allocateBuffer)
            {
                outputBuffer.OwnBuffer(
                    alloc,
                    PG_NEW_ARRAY(alloc, -1, "file buffer", Pegasus::Alloc::PG_MEM_PERM, char, fileSize.LowPart),
                    fileSize.LowPart
                );
            }
            else if (outputBuffer.GetFileSize() > outputBuffer.GetBufferSize())
            {
                CloseHandle(fileHandle);
                return Pegasus::Io::ERR_BUFFER_TOO_SMALL;
            }

            outputBuffer.SetFileSize(fileSize.LowPart);
            DWORD bytesRead = 0;
            ReadFile(fileHandle, outputBuffer.GetBuffer(), outputBuffer.GetFileSize(), &bytesRead, NULL); 
            
            PG_ASSERTSTR(bytesRead == outputBuffer.GetFileSize(), "bytes read do not match bytes requested, file %s", path);
            CloseHandle(fileHandle);
            if (allocateBuffer && GetLastError() != 0)
            {
                outputBuffer.DestroyBuffer();
                return Pegasus::Io::ERR_READING_FILE;
            }
        }
    }
    else
    {
        PG_LOG('FILE', "File not found \"%s\"", path);

        return Pegasus::Io::ERR_FILE_NOT_FOUND;
    }
    PG_LOG('FILE', "Successfully opened file \"%s\"", path);

    return Pegasus::Io::ERR_NONE;
}

Pegasus::Io::IoError NativeSaveBufferToFile(const char* path, const Pegasus::Io::FileBuffer& outputBuffer)
{
    HANDLE fileHandle = CreateFile(
                            path,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                        );

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        PG_LOG('FILE', "IO Error (CreateFile): %s", path);
        return Pegasus::Io::ERR_OPENING_FILE;
    }
    
    DWORD bytesWritten = 0;
    BOOL res = WriteFile (
        fileHandle,
        outputBuffer.GetBuffer(),
        outputBuffer.GetFileSize(),
        &bytesWritten,
        NULL
    );
    SetEndOfFile(fileHandle);
    CloseHandle(fileHandle);
    DWORD lastErr = GetLastError();
    if (!res || (GetLastError() != 0 && GetLastError() != ERROR_ALREADY_EXISTS))
    {
        PG_LOG('FILE', "IO Error (WriteFile): %s", path);
        return Pegasus::Io::ERR_WRITING_FILE;
    }
    PG_LOG('FILE', "Saved: %s", path);
    return Pegasus::Io::ERR_NONE;
}

#else
    #error No native implementation for IO functions in current platform!
#endif //platform selection
}// namespace internal
#endif //PEGASUS_USE_NATIVE_CALLS

//----------------------------------------------------------------------------------------

IOManager::IOManager(const char* rootPath)
{
    // Configure the path
    sprintf_s(mRootDirectory, MAX_FILEPATH_LENGTH, "%s", rootPath); // Hardcode imported for now
    PG_LOG('FILE', "Asset root set to \"%s\"", mRootDirectory);
}

//----------------------------------------------------------------------------------------

IOManager::~IOManager()
{
}

//----------------------------------------------------------------------------------------

IoError IOManager::OpenFileToBuffer(const char* relativePath, FileBuffer& outputBuffer, bool allocateBuffer, Alloc::IAllocator* alloc)
{
    char pathBuffer[MAX_FILEPATH_LENGTH];

    // Configure the path
    pathBuffer[0] = '\0';
    PG_ASSERTSTR(Pegasus::Utils::Strlen(relativePath) < MAX_FILEPATH_LENGTH, "Path str is too little! be prepared for some mem stomps!");
    Pegasus::Utils::Strcat(pathBuffer, mRootDirectory);
    pathBuffer[MAX_FILEPATH_LENGTH - 1] = '\0';
    Pegasus::Utils::Strcat(pathBuffer, relativePath);
    pathBuffer[MAX_FILEPATH_LENGTH - 1] = '\0';

    // Load the file
#if PEGASUS_USE_NATIVE_IO_CALLS
    return internal::NativeOpenFileToBuffer(pathBuffer, outputBuffer, allocateBuffer, alloc); 
#else
    //default to c runtime file functions
    FILE * fileHandle = nullptr;

    fopen_s(&fileHandle, pathBuffer, "rb");
    if (fileHandle)
    {
        fseek(fileHandle, 0L, SEEK_END);
        long int fullSize = ftell(fileHandle);
        fseek(fileHandle, 0L, SEEK_SET);

        if (fullSize > 0x00000000FFFFFFFF)
        {
            PG_FAILSTR("Pegasus does not support files bigger than 4 gb, file %s", pathBuffer);
            fclose(fileHandle);
            return ERR_FILE_SIZE_TOO_BIG;
        }

        int fileSize = static_cast<int>(fullSize); //intentionally clamp file size
        if (allocateBuffer)
        {
            outputBuffer.OwnBuffer (
                alloc,
                PG_NEW_ARRAY(alloc, -1, "file buffer", Pegasus::Alloc::PG_MEM_PERM, char, fileSize),
                fileSize
            );
        }
        else if (outputBuffer.GetFileSize() > outputBuffer.GetBufferSize())
        {
            fclose(fileHandle);
            return Pegasus::Io::ERR_BUFFER_TOO_SMALL;
        }
        outputBuffer.SetFileSize(fileSize);//force loss presicion
        int bytesRead = fread(outputBuffer.GetBuffer(), 1/*byte*/, outputBuffer.GetFileSize(), fileHandle);
        PG_ASSERTSTR(bytesRead == outputBuffer.GetFileSize(), "bytes read do not match bytes requested, file %s", pathBuffer);
        fclose(fileHandle);
        if (allocateBuffer && ferror(fileHandle))
        {
            outputBuffer.DestroyBuffer();
            return Pegasus::Io::ERR_READING_FILE;
        }

    } 
    else
    {
        PG_LOG('FILE', "File not found \"%s\"", pathBuffer);

        return Pegasus::Io::ERR_FILE_NOT_FOUND;
    }

    PG_LOG('FILE', "Successfully opened file \"%s\"", pathBuffer);
    return Pegasus::Io::ERR_NONE; 
#endif
}

//----------------------------------------------------------------------------------------


Pegasus::Io::IoError Pegasus::Io::IOManager::SaveFileToBuffer(const char* relativePath, const Pegasus::Io::FileBuffer& inputBuffer)
{
    //todo - implement saving to a file :)
    char pathBuffer[MAX_FILEPATH_LENGTH];

    // Configure the path
    pathBuffer[0] = '\0';
    PG_ASSERTSTR(Pegasus::Utils::Strlen(relativePath) < MAX_FILEPATH_LENGTH, "Path str is too little! be prepared for some mem stomps!");
    Pegasus::Utils::Strcat(pathBuffer, mRootDirectory);
    pathBuffer[MAX_FILEPATH_LENGTH - 1] = '\0';
    Pegasus::Utils::Strcat(pathBuffer, relativePath);
    pathBuffer[MAX_FILEPATH_LENGTH - 1] = '\0';

#if PEGASUS_USE_NATIVE_IO_CALLS
    return internal::NativeSaveBufferToFile(pathBuffer, inputBuffer);
#else
    FILE * fileHandle = nullptr;
    fopen_s(&fileHandle, pathBuffer, "wb");
    if (fileHandle == 0)
    {
        PG_LOG('FILE', "IO Error (fopen): %s", pathBuffer);
        return Pegasus::Io::ERR_OPENING_FILE;
    }
    size_t v = fwrite(inputBuffer.GetBuffer(), 1, inputBuffer.GetFileSize(), fileHandle);
    fclose(fileHandle);    
    if (static_cast<int>(v) != inputBuffer.GetFileSize())
    {        
         PG_LOG('FILE', "IO Error (fwrite): %s", pathBuffer);
         return Pegasus::Io::ERR_WRITING_FILE;
    }
    PG_LOG('FILE', "Saved: %s", pathBuffer);
    return Pegasus::Io::ERR_NONE;
    
#endif

}

//----------------------------------------------------------------------------------------

Pegasus::Io::FileBuffer::FileBuffer()
:   mAllocator(nullptr),
    mBuffer(nullptr), 
    mFileSize(0), 
    mBufferSize(0)
{
}

//----------------------------------------------------------------------------------------

Pegasus::Io::FileBuffer::~FileBuffer()
{
    DestroyBuffer();
}

//----------------------------------------------------------------------------------------

void Pegasus::Io::FileBuffer::OwnBuffer(Alloc::IAllocator* bufferAlloc, char * buffer, int bufferSize)
{
    PG_ASSERTSTR(mBuffer == nullptr, "Dangerous operation! please call ForgetBuffer or DestroyBuffer before Setting a new buffer");
    mAllocator = bufferAlloc;
    mBuffer = buffer;
    mBufferSize = bufferSize;
    mFileSize = bufferSize;
}

//----------------------------------------------------------------------------------------

void Pegasus::Io::FileBuffer::ForgetBuffer()
{
    mAllocator = nullptr;
    mBuffer = nullptr;
    mBufferSize = 0;
    mFileSize = 0;
}

//----------------------------------------------------------------------------------------

void Pegasus::Io::FileBuffer::DestroyBuffer()
{
    PG_DELETE_ARRAY(mAllocator, mBuffer);

    mAllocator = nullptr;
    mBuffer = nullptr;
    mBufferSize = 0;
    mFileSize = 0;
}

//----------------------------------------------------------------------------------------

void Pegasus::Io::FileBuffer::SetFileSize(int fileSize)
{
    PG_ASSERTSTR(fileSize <= mBufferSize, "the file size must be lesser than the buffer containing it");
    mFileSize = fileSize;
}

} // namespace Io
} // namespace Pegasus
