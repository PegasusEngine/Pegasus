/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Ref.h
//! \author Kevin Boulanger
//! \date   17th October 2013
//! \brief  Pointer to a reference counted object

#ifndef PEGASUS_CORE_REF_H
#define PEGASUS_CORE_REF_H

namespace Pegasus {
namespace Core {

//! Pointer to a reference counted object of class C
//! (intrusive approach, the pointed object needs to have a predefined set of functions).
//! \warning The pointed class must have a reference counter initialized to 0
//!          and provide the following private functions (with a friend access):
//!          void AddRef();             -> Adds 1 to the reference counter
//!          int GetRefCount() const;   -> Returns the reference counter, useful in DEV mode only
//!          void Release();            -> Decreases the reference counter, and if it reaches 0, self-destroy
template <class C>
class Ref
{
public:
    //! Default constructor, equivalent to nullptr
    Ref() : mObject(nullptr)
    {
    }

    //! Constructor using a weak pointer
    //! \param ptr Weak pointer to the object for which the reference becomes the owner
    //! \note Increases the reference counter of the pointed object if defined
    //! \warning The input pointer is considered as a weak pointer,
    //!          meaning that the object's reference counter should be 0
    Ref(C * ptr) : mObject(nullptr)
    {
        *this = ptr;
    }

    //! Copy constructor
    //! \param ref Reference to copy
    //! \note Increases the reference counter of the pointed object if defined
    Ref(const Ref<C> & ref) : mObject(nullptr)
    {
        *this = ref;
    }

    //! Destructor
    //! \note Decreases the reference counter of the currently pointed object if defined
    ~Ref()
    {
        if (mObject != nullptr)
        {
            mObject->Release();
        }
    }

    //! Assignment operator with a weak pointer
    //! \param ptr Pointer to copy
    //! \note Decreases the reference counter of the currently pointed object if defined
    //!       and increases the reference counter of the new pointed object if defined
    //! \warning The input pointer is considered as a weak pointer,
    //!          meaning that the object's reference counter should be 0
    Ref<C> & operator=(C * ptr)
    {
        if (mObject != ptr)
        {
            if (mObject != nullptr)
            {
                mObject->Release();
            }
            if (ptr != nullptr)
            {
                // In most cases, this assertion test makes sense,
                // but in the cast of a cast (see operator below), the counter can be > 0
                //PG_ASSERTSTR(ptr->GetRefCount() == 0, "Invalid object given to the reference, the object has a reference count of %d, but it should 0", ptr->GetRefCount());

                ptr->AddRef();
            }
            mObject = ptr;
        }
        return *this;
    }

    //! Assignment operator with another reference
    //! \param ref Reference to copy
    //! \note Decreases the reference counter of the currently pointed object if defined
    //!       and increases the reference counter of the new pointed object if defined
    Ref<C> & operator=(const Ref<C> & ref)
    {
        if (mObject != ref.mObject)
        {
            if (mObject != nullptr)
            {
                mObject->Release();
            }
            if (ref.mObject != nullptr)
            {
                ref.mObject->AddRef();
            }
            mObject = ref.mObject;
        }
        return *this;
    }


    //! Dereference operator
    //! \return Reference to the pointed object
    C & operator*()
    {
#if PEGASUS_ENABLE_DETAILED_ASSERT
        PG_ASSERTSTR(mObject != nullptr, "Trying to dereference a null Ref<>");
#endif
        return *mObject;
    }

    //! Dereference operator (const version)
    //! \return Reference to the pointed object
    const C & operator*() const
    {
#if PEGASUS_ENABLE_DETAILED_ASSERT
        PG_ASSERTSTR(mObject != nullptr, "Trying to dereference a null Ref<>");
#endif
        return *mObject;
    }

    //! Pointer member access operator
    //! \return Pointed to the pointed object
    C * operator->()
    {
#if PEGASUS_ENABLE_DETAILED_ASSERT
        PG_ASSERTSTR(mObject != nullptr, "Trying to access a member of a null Ref<>");
#endif
        return mObject;
    }

    //! Pointer member access operator (const version)
    //! \return Pointed to the pointed object
    const C * operator->() const
    {
#if PEGASUS_ENABLE_DETAILED_ASSERT
        PG_ASSERTSTR(mObject != nullptr, "Trying to access a member of a null Ref<>");
#endif
        return mObject;
    }

  
    //! Reference cast operator, with C2 as the destination class
    //! \warning C2 must be a class derived from C
    //! \return Reference to the object cast to a different type
    template <class C2>
    inline operator Ref<C2>() const { return Ref<C2>(static_cast<C2 *>(mObject)); }

    //! Weak pointer cast operator, with C2 as the destination class
    //! \warning C2 must be a class derived from C
    //! \warning The returned pointer is weak, meaning that no reference counting is handled for it
    //! \return Pointer to the object cast to a different pointer type
    template <class C2>
    inline operator C2 *() { return static_cast<C2 *>(mObject); }

    //! Constant weak pointer cast operator, with C2 as the destination class
    //! \warning C2 must be a class derived from C
    //! \warning The returned pointer is weak, meaning that no reference counting is handled for it
    //! \return Pointer to the object cast to a different pointer type
    template <class C2>
    inline operator const C2 *() const { return static_cast<const C2 *>(mObject); }


    //! Comparison operator with a weak pointer
    //! \warning Compares the pointers, not the objects themselves
    //! \param ptr Pointer to compare with
    //! \return True if the pointers are equal (and not the pointed objects)
    //! \note Comparing a null reference to a null input pointer is considered valid and returns true
    bool operator==(C * ptr) const { return (mObject == ptr); }

    //! Comparison operator with another reference
    //! \warning Compares the pointers, not the objects themselves
    //! \param ref Reference to compare with
    //! \return True if the pointers are equal (and not the pointed objects)
    //! \note Comparing a null reference to another null reference is considered valid and returns true
    bool operator==(const Ref<C> & ref) const { return (mObject == ref.mObject); }

    //! Difference operator with a weak pointer
    //! \warning Compares the pointers, not the objects themselves
    //! \param ptr Pointer to compare with
    //! \return True if the pointers are different (and not the pointed objects)
    //! \note Comparing a null reference to a null input pointer is considered valid and returns false
    bool operator!=(C * ptr) const { return (mObject != ptr); }

    //! Difference operator with another reference
    //! \warning Compares the pointers, not the objects themselves
    //! \param ref Reference to compare with
    //! \return True if the pointers are different (and not the pointed objects)
    //! \note Comparing a null reference to another null reference is considered valid and returns false
    bool operator!=(const Ref<C> & ref) const { return (mObject != ref.mObject); }

    //! Inversion operator, typically used when testing if a reference is empty (!ref)
    //! \return True if the reference is empty
    bool operator!() const { return (mObject == nullptr); }


private:
    //! Pointer to the reference counted object, nullptr when undefined
    C * mObject;
};


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_REF_H
