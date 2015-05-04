/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridStaticAllocator.h
//! \author	Kevin Boulanger
//! \date	05th February 2015
//! \brief	Allocator for the static objects used by property grids, such as the manager

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRIDSTATICALLOCATOR_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRIDSTATICALLOCATOR_H

#include "Pegasus/Allocator/IAllocator.h"

namespace Pegasus {
namespace PropertyGrid {


//! Allocator for the static objects used by property grids, such as the manager.
//! It has to be separate from the other allocators as it is statically allocated
//! before main() is called
class PropertyGridStaticAllocator : public Alloc::IAllocator
{
public:

    //! Constructor
    PropertyGridStaticAllocator();

    //! Get the unique instance of the property grid static allocator
    //! \return Reference to the unique instance of the property grid static allocator
    static PropertyGridStaticAllocator & GetInstance();

    // IAllocator interface
    virtual void * Alloc(size_t size, Alloc::Flags flags, Alloc::Category category, const char * debugText, const char* file, unsigned int line);
    virtual void * AllocAlign(size_t size, Alloc::Alignment align, Alloc::Flags flags, Alloc::Category category, const char * debugText, const char * file, unsigned int line);
    virtual void Delete(void * ptr);

private:

    //! Destructor
    virtual ~PropertyGridStaticAllocator();

    // No copies allowed
    PG_DISABLE_COPY(PropertyGridStaticAllocator);
};


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRIDSTATICALLOCATOR_H
