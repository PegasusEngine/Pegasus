/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ByteStream.h
//! \author Kleber Garcia
//! \date   2/10/2015
//! \brief  Pegasus resizable byte stream. Use this class to add elements to a constant growing
//!         vector

#ifndef PEGASUS_BYTE_STREAM_H
#define PEGASUS_BYTE_STREAM_H

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace Utils
{
    
    // byte stream class containing byte operations
    class ByteStream
    {
    public:
        //! Constructor of byte stream
        explicit ByteStream(Alloc::IAllocator* allocator);

        //! Destructor of byte stream
        ~ByteStream();

        //! Gets the raw buffer
        void* GetBuffer() { return mBuffer; }

        //! Gets the raw buffer
        const void* GetBuffer() const { return mBuffer; }

        //! Removes total ownership of the current buffer (it also resets the state of this object). It assumes somebody else will reference it and destroy it.
        //! \warning The buffer must be acquired before calling this function. If this function is called and nobody else destroys the buffer, then it will result in a memory leak.
        void ForgetBuffer();

        //! Gets the total internal buffer size
        int GetSize() const { return mBufferSize; }

        //! Appends an element to the stream
        void Append(const void* buffer, int size); 

        //! Appends another buffer stream to this stream
        void Append(const ByteStream* stream);

        //! Resets buffer and deletes any accumulated memory
        void Reset();


    private:
        Alloc::IAllocator* mAllocator;
        void* mBuffer;
        int   mBufferSize;
        int   mTotalCount;
    };
}

}

#endif
