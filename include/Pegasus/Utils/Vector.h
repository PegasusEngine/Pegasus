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

#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/TypeTraits.h"
#include "Pegasus/Memory/MemoryManager.h"


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
    BaseVector(Alloc::IAllocator* allocator, unsigned int typeSize);

    //! Destructor
    ~BaseVector();

    //! \return size of elements
    unsigned int GetSize() const { return mDataSize; }

    //! \return the allocator
    Alloc::IAllocator* GetAlloc() { return mAlloc; }

    //! Stores element into the target buffer
    void* GetElement(unsigned int index) {
        PG_ASSERT(index < mDataSize);
        return static_cast<void*>(static_cast<char*>(mData) + index * mElementByteSize); 
    }

    //! Gets element into the target buffer
    const void* GetElement(unsigned int index) const {
        PG_ASSERT(index < mDataSize);
        return static_cast<void*>(static_cast<char*>(mData) + index * mElementByteSize); 
    }

    //! deletes element at specified index
    void Delete(unsigned int index);

    //! Pushes an empty object and returns its pointer
    void* PushEmpty();

    //! Deletes all data
    void Clear();

    //! \return gets the raw data pointer of this vector
    void* Data() { return mData; }

    //! \return gets the raw data pointer of this vector
    const void* Data() const { return mData; }

private:
    //! master data pointer
    void* mData;

    //! size of the element
    unsigned int mElementByteSize;

    //! capacity of vector
    unsigned int mDataCount;

    //! the current size of the vector
    unsigned int mDataSize;

    //! the allocator
    Alloc::IAllocator* mAlloc;
};

//! The vector convenience template class
template<class T>
class Vector
{
public:
    //! Constructor
    explicit Vector(Alloc::IAllocator* alloc) : mBase(alloc, sizeof(T)) {}

    Vector() : mBase(Memory::GetGlobalAllocator(), sizeof(T)) {}

    //! Destructor
    ~Vector()
    {
        // Call Clear() to force the destructors to be called for complex types
        Clear();
    }

    //! Gets the size
    inline unsigned int GetSize() const { return mBase.GetSize(); }

    //! [] operator, just like an array
    inline T& operator[](unsigned int index) 
    {
        T* t = reinterpret_cast<T*>(mBase.GetElement(index));
        return *t;
    }

    //! [] operator, just like an array
    inline const T& operator[](unsigned int index) const
    {
        const T* t = reinterpret_cast<const T*>(mBase.GetElement(index));
        return *t;
    }

    //! creates and pushes a new element
    T& PushEmpty()
    {
        T* v = static_cast<T*>(mBase.PushEmpty());
        if (TypeTraits<T>::IsPOD)
        {
            // If the type T is plain old data, just call the standard initialization
            new (v) T;
        }
        else
        {
#pragma warning(push)    
#pragma warning(disable:4345)   // Behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
                                // This is a VStudio 2005 to 2012 obsolete warning
            // If the type T is complex and has a default constructor, call it
            new (v) T();
#pragma warning(pop)
        }
        return *v;
    }

    T Pop()
    {
        // Use a reference to avoid creating a local copy, which then gets copied again when returned.
        T& val = (*this)[GetSize() - 1];
        Delete(GetSize() - 1);
        return val;
    }

    //! deletes element at specified index
    void Delete(unsigned int i)
    {
        if (!TypeTraits<T>::IsPOD)
        {
            // Call the destructor only for complex types
            ((*this)[i]).~T();
        }
        mBase.Delete(i);
    }

    void Clear()
    {
        if (!TypeTraits<T>::IsPOD)
        {
            // Call the destructor of each element for complex types
            const unsigned int size = GetSize();
            for (unsigned int i = 0; i < size; ++i)
            {
                ((*this)[i]).~T();
            }
        }
        mBase.Clear();
    }

    T* Data()
    {
        return static_cast<T*>(mBase.Data());
    }

    const T* Data() const
    {
        return static_cast<const T*>(mBase.Data());
    }

private:
    BaseVector mBase;

    
};


}
}


#endif
