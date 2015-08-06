/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridStaticAllocator.cpp
//! \author	Karolyn Boulanger
//! \date	05th February 2015
//! \brief	Allocator for the static objects used by property grids, such as the manager

#include "Pegasus/PropertyGrid/PropertyGridStaticAllocator.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridStaticAllocator::PropertyGridStaticAllocator()
{
}

//----------------------------------------------------------------------------------------

PropertyGridStaticAllocator::~PropertyGridStaticAllocator()
{
}

//----------------------------------------------------------------------------------------

PropertyGridStaticAllocator & PropertyGridStaticAllocator::GetInstance()
{
    // Return the singleton
    static PropertyGridStaticAllocator sInstance;
    return sInstance;
}

//----------------------------------------------------------------------------------------

void * PropertyGridStaticAllocator::Alloc(size_t size, Alloc::Flags flags, Alloc::Category category, const char * debugText, const char * file, unsigned int line)
{
    return malloc(size);
}

//----------------------------------------------------------------------------------------

void * PropertyGridStaticAllocator::AllocAlign(size_t size, Alloc::Alignment align, Alloc::Flags flags, Alloc::Category category, const char * debugText, const char * file, unsigned int line)
{
    return malloc(size);
}

//----------------------------------------------------------------------------------------

void PropertyGridStaticAllocator::Delete(void * ptr)
{
    free(ptr);
}


}   // namespace PropertyGrid
}   // namespace Pegasus
