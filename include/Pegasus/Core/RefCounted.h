/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RefCounted.h
//! \author Kleber Garcia
//! \date   August 2nd 2015
//! \brief  Refcounted object (basic refcount operations and state tracking) 


#ifndef PEGASUS_CORE_REFCOUNTED_H
#define PEGASUS_CORE_REFCOUNTED_H

namespace Pegasus {
    namespace Alloc {
        class IAllocator;
    }
}

namespace Pegasus {
namespace Core {

class RefCounted
{
public:
    
    //! Constructor
    explicit RefCounted(Alloc::IAllocator* allocator);

    //! Destructor
    virtual ~RefCounted();

    //! Increment the reference counter, used by Ref<Node>
    inline void AddRef() { mRefCount++; }

    //! Decrease the reference counter, and delete the current object
    //! if the counter reaches 0
    void Release();

    //! Get the current reference count of this object
    //! \return the ref count
    inline int GetRefCount() const { return mRefCount; }

private:

    //! Reference counter
    int mRefCount;
    
    //! Pointer to allocator
    Alloc::IAllocator* mAllocator;
};

}
}

#endif
