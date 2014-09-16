/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Containre.h
//! \author Kleber Garcia
//! \date   September 8th 2014
//! \brief  BlockScript container for variable size data

#ifndef PEGASUS_BS_CONTAINER
#define PEGASUS_BS_CONTAINER

#include "Pegasus/BlockScript/AstAllocator.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Assertion.h"
#include <new>

#define CONTAINER_PAGE_SZ 32

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace BlockScript 
{
    //! Container class, reuses as much memory as possible for rapid empting and rapid filling
    template<class T>
    class Container : private AstAllocator
    {
    public:

        //! constructor
        Container() {}

        //! destructor
        virtual ~Container();

        //! to be called to initialize memory
        //! \param alloc the allocator to use
        void Initialize(Alloc::IAllocator* alloc);

        //! operator that gets a read / write reference
        T& operator[] (int idx);

        //! operator that gets a read / write reference
        const T& operator[] (int idx) const;

        //! Gets the size of this container
        //! \param number of parameters
        int Size() const;

        //! Resets the container, sets the count to 0. Does not free memory.
        void Reset(); 

        //! Pushes empty element
        //! \return the unallocated structure to use. Calls empty constructor of such structure
        T& PushEmpty();
    };

    template<class T>
    void Container<T>::Initialize(Alloc::IAllocator* alloc)
    {
        AstAllocator::Initialize(sizeof(T) * CONTAINER_PAGE_SZ, alloc); 
    }

    template<class T>
    T& Container<T>::PushEmpty()
    {
		void* mem = Alloc(
                static_cast<size_t>(sizeof(T)),
				Alloc::PG_MEM_TEMP,
                -1
        );

		return *(new(mem) T);
        
    }

    template<class T>
    void Container<T>::Reset()
    { 
        int s = Size();
        for (int i = 0; i < s; ++i)
        {
            T& t = (*this)[i];
            t.~T();
        }
		AstAllocator::Reset();
    }

    template<class T>
    T& Container<T>::operator[] (int i)
    {
        int s = Size();
        PG_ASSERT(i >= 0 && i < s);
        char** ps = GetMemoryPages();
        int memsz = GetMemorySize();
        int pageSize = GetPageSize();
		int offset = (i * sizeof(T));
        int pageId =  offset / (pageSize);
        char* page = ps[pageId];
        return *reinterpret_cast<T*>(page + (offset % pageSize));
    };

    template<class T>
    const T& Container<T>::operator[] (int i) const
    {
        int s = Size();
        PG_ASSERT(i >= 0 && i < s);
        char** const ps = GetMemoryPagesConst();
        int memsz = GetMemorySize();
        int pageSize = GetPageSize();
		int offset = (i * sizeof(T));
        int pageId =  offset / (pageSize);
        const char* page = ps[pageId];
        return *reinterpret_cast<const T*>(page + (offset % pageSize));
    };

    template <class T>
    int Container<T>::Size() const
    {
        return GetMemorySize() / sizeof(T);
    }

	template <class T>
    Container<T>::~Container()
    {
        Reset();
    }
};

}

#endif
