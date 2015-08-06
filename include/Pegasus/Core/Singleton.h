/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Singleton.h
//! \author Karolyn Boulanger
//! \date   24th June 2013
//! \brief  Singleton base classes (manual and automatic creation)

#ifndef PEGASUS_CORE_SINGLETON_H
#define PEGASUS_CORE_SINGLETON_H

#include "Pegasus/Allocator/Alloc.h"

#define _STRINGIFY_SINGLETON_CLASS(C) #C

namespace Pegasus {
namespace Core {

//! Singleton template
//! Base class for an object that has to exist only once in the application.
//! Creation and destruction have to be handled manually.
//! \warning A GetInstance() can return nullptr, so be careful when using the instance pointer
template <class C>
class Singleton
{
public:
    //! Create the unique instance of the class
    //! \param alloc Allocator used to create the instance.
    //! \return Pointer to the unique instance of the class.
    //! \warning Cannot be called twice without a call to \a DestroyInstance() first
    static C* CreateInstance(Alloc::IAllocator* alloc);

    //! Destroy the unique instance of the class
    //! \warning \a CreateInstace() must be called at least once before this function
    static void DestroyInstance();

    //! Get the unique instance of the class
    //! \return Pointer to the unique instance of the class,
    //!         nullptr is \a CreateInstance() has not been called yet
    static C * GetInstance();

protected:
    //! Constructor, protected to allow only the derived class' constructor to call it
    Singleton() { };

    //! Destructor, protected to allow only the derived class' destructor to call it
    virtual ~Singleton() { };

private:
    // Singletons cannot be copied
    PG_DISABLE_COPY(Singleton)

    static C* sInstance; //!< Unique instance of the class, nullptr by default
    static Alloc::IAllocator* sAllocator; //!< Allocator used to create the instance
};


//----------------------------------------------------------------------------------------

template<typename C>
C* Singleton<C>::sInstance = nullptr;
template<typename C>
Alloc::IAllocator* Singleton<C>::sAllocator = nullptr;

//----------------------------------------------------------------------------------------

template <class C>
C* Singleton<C>::CreateInstance(Alloc::IAllocator* alloc)
{
    // Sanity check
    PG_ASSERTSTR(sInstance == nullptr, "CreateInstance called without DestroyInstance!");

    sInstance = PG_NEW(alloc, -1, _STRINGIFY_SINGLETON_CLASS(C), Pegasus::Alloc::PG_MEM_PERM) C();
    sAllocator = alloc;

    return sInstance;
}

//----------------------------------------------------------------------------------------

template <class C>
void Singleton<C>::DestroyInstance()
{
    // Sanity check
    PG_ASSERTSTR(sInstance != nullptr, "DestroyInstance called without CreateInstance called multiple times!");

    // Delete it
    PG_DELETE(sAllocator, sInstance);
    sAllocator = nullptr;
    sInstance = nullptr;
}

//----------------------------------------------------------------------------------------

template <class C>
C* Singleton<C>::GetInstance()
{
    return sInstance;
}


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_SINGLETON_H
