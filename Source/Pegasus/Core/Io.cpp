

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

//using the windows libraries to produce file IO
#include "stdio.h"
#if PEGASUS_USE_NATIVE_IO_CALLS
#if PEGASUS_PLATFORM_WINDOWS
#include <windows.h>
#endif
#endif


//implementation using native windows api calls
#if PEGASUS_USE_NATIVE_IO_CALLS
namespace PegasusIoPrivate
{
#if PEGASUS_PLATFORM_WINDOWS

Pegasus::Io::IoError NativeOpenFileToBuffer(const char * relativePath, Pegasus::Io::FileBuffer& outputBuffer, bool allocateBuffer)
{
    HANDLE fileHandle = CreateFile(
        relativePath,
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
                    PG_CORE_NEW("file buffer", Pegasus::Memory::PG_MEM_PERM) char[fileSize.LowPart],
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
            
            PG_ASSERTSTR(bytesRead == outputBuffer.GetFileSize(), "bytes read do not match bytes requested, file %s", relativePath);
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
        return Pegasus::Io::ERR_FILE_NOT_FOUND;
    }
    //enable low level logging, logging is failing
    //PG_LOG('FILE', "successfully opened file \"%s\"", relativePath);
    return Pegasus::Io::ERR_NONE;
}

#else
    #error No native implementation for IO functions in current platform!
#endif //platform selection
}// namespace PegasusIoPrivate
#endif //PEGASUS_USE_NATIVE_CALLS

//implementation using the c runtime calls
Pegasus::Io::IoError Pegasus::Io::OpenFileToBuffer(const char * relativePath, Pegasus::Io::FileBuffer& outputBuffer, bool allocateBuffer)
{
#if PEGASUS_USE_NATIVE_IO_CALLS
    return PegasusIoPrivate::NativeOpenFileToBuffer(relativePath, outputBuffer, allocateBuffer); 
#else
    //default to c runtime file functions
    FILE * fileHandle = nullptr;

    fopen_s(&fileHandle, relativePath, "rb");
    if (fileHandle)
    {
        fseek(fileHandle, 0L, SEEK_END);
        long int fullSize = ftell(fileHandle);
        fseek(fileHandle, 0L, SEEK_SET);

        if (fullSize > 0x00000000FFFFFFFF)
        {
            PG_FAILSTR("Pegasus does not support files bigger than 4 gb!");
            fclose(fileHandle);
            return ERR_FILE_SIZE_TOO_BIG;
        }

        int fileSize = static_cast<int>(fullSize); //intentionally clamp file size
        if (allocateBuffer)
        {
            outputBuffer.OwnBuffer (
                PG_CORE_NEW("file buffer", Pegasus::Memory::PG_MEM_PERM) char[fileSize],
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
        PG_ASSERTSTR(bytesRead == outputBuffer.GetFileSize(), "bytes read do not match bytes requested, file %s", relativePath);
        fclose(fileHandle);
        if (allocateBuffer && ferror(fileHandle))
        {
            outputBuffer.DestroyBuffer();
            return Pegasus::Io::ERR_READING_FILE;
        }

    } 
    else
    {
        return Pegasus::Io::ERR_FILE_NOT_FOUND;
    }
    //TODO: enable low level logging, logger is failing
    //PG_LOG('FILE', "successfully opened file \"%s\"", relativePath);
    return Pegasus::Io::ERR_NONE; 
#endif
}

Pegasus::Io::FileBuffer::FileBuffer()
:   mBuffer(nullptr), 
    mFileSize(0), 
    mBufferSize(0)
{
}

Pegasus::Io::FileBuffer::~FileBuffer()
{
    DestroyBuffer();
}

void Pegasus::Io::FileBuffer::OwnBuffer(char * buffer, int bufferSize)
{
    PG_ASSERTSTR(mBuffer == nullptr, "Dangerous operation! please call ForgetBuffer or DestroyBuffer before Setting a new buffer");
    mBuffer = buffer;
    mBufferSize = bufferSize;
}

void Pegasus::Io::FileBuffer::ForgetBuffer()
{
    mBuffer = nullptr;
    mBufferSize = 0;
    mFileSize = 0;
}

void Pegasus::Io::FileBuffer::DestroyBuffer()
{
    if (mBuffer != nullptr)
    {
        PG_DELETE[] mBuffer;
        mBuffer = nullptr;
        mBufferSize = 0;
        mFileSize = 0;
    }
}

void Pegasus::Io::FileBuffer::SetFileSize(int fileSize)
{
    PG_ASSERTSTR(fileSize <= mBufferSize, "the file size must be lesser than the buffer containing it");
    mFileSize = fileSize;
}

