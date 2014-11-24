/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Vector.h
//! \author Kleber Garcia
//! \date   November 2nd 2014
//! \brief  Pegasus resizable vector. Favoring small code for performance

#ifndef PEGASUS_UTILS_VECTOR_H
#define PEGASUS_UTILS_VECTOR_H

#include "Pegasus/Core/Assertion.h"

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace Utils
{

//!The vector container class
class BaseVector
{
public:

    //! Constructor
    BaseVector(Alloc::IAllocator* allocator, int typeSize);

    //! Destructor
    ~BaseVector();

    //! \return size of elements
    int GetSize() const { return mDataSize; }

    //! \return the allocator
    Alloc::IAllocator* GetAlloc() { return mAlloc; }

    //! Stores element into the target buffer
    void* GetElement(int index) {
        PG_ASSERT(index < mDataSize);
        return static_cast<void*>(static_cast<char*>(mData) + index * mElementByteSize); 
    }

    //! Gets element into the target buffer
    const void* GetElement(int index) const {
        PG_ASSERT(index < mDataSize);
        return static_cast<void*>(static_cast<char*>(mData) + index * mElementByteSize); 
    }

    //! deletes element at specified index
    void Delete(int index);

    //! Pushes an empty object and returns its pointer
    void* PushEmpty();

    //! Deletes all data
    void Clear();

private:
    //! master data pointer
    void* mData;

    //! size of the element
    int mElementByteSize;

    //! capacity of vector
    int mDataCount;

    //! the current size of the vector
    int mDataSize;

    //! the allocator
    Alloc::IAllocator* mAlloc;
};

//! The vector convinience template class
template<class T>
class Vector
{
public:
    //! Constructor
    explicit Vector(Alloc::IAllocator* alloc) : mBase(alloc, sizeof(T)) {}

    //! Destructor
    ~Vector(){}

    //! Gets the size
    int GetSize() const { return mBase.GetSize(); }

    //! [] operator, just like an array
    T& operator[](int index) 
    {
        T* t = reinterpret_cast<T*>(mBase.GetElement(index));
        return *t;
    }

    //! [] operator, just like an array
    const T& operator[](int index) const
    {
        const T* t = reinterpret_cast<const T*>(mBase.GetElement(index));
        return *t;
    }

    //! creates and pushes a new element
    T& PushEmpty()
    {
        T* v = static_cast<T*>(mBase.PushEmpty());
        return *v;
    }

    //! deletes element at specified index
    void Delete(int i)
    {
        mBase.Delete(i);
    }

    void Clear()
    {
        mBase.Clear();
    }

private:
    BaseVector mBase;

    
};


}
}


#endif
