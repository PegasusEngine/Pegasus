/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObject.cpp
//! \author	Kevin Boulanger
//! \date	10th February 2015
//! \brief	Property grid object, parent of every class that defines a set of editable properties

#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace PropertyGrid {


void PropertyAccessor::Read(void * outputBuffer, unsigned int outputBufferSize) const
{
    PG_ASSERTSTR(outputBuffer != nullptr, "Trying to read a property into a null buffer.");
    PG_ASSERTSTR(outputBufferSize > 0, "Trying to read a property into a buffer with an undefined size.");
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    PG_ASSERTSTR(outputBufferSize == mSize, "Trying to read a property into a buffer whose size is incorrect.");
#endif

    //! \todo Use a fast memcopy function that always take the fast path
    Utils::Memcpy(outputBuffer, mPtr, outputBufferSize);
}

//----------------------------------------------------------------------------------------

void PropertyAccessor::Write(const void * inputBuffer, unsigned int inputBufferSize) const
{
    PG_ASSERTSTR(inputBuffer != nullptr, "Trying to write a property from a null buffer.");
    PG_ASSERTSTR(inputBufferSize > 0, "Trying to write a property from a buffer with an undefined size.");
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    PG_ASSERTSTR(inputBufferSize == mSize, "Trying to write a property from a buffer whose size is incorrect.");
#endif

    //! \todo Use a fast memcopy function that always take the fast path
    Utils::Memcpy(mPtr, inputBuffer, inputBufferSize);
    mObj->InvalidatePropertyGrid();
}

//----------------------------------------------------------------------------------------

BEGIN_IMPLEMENT_PROPERTIES2(PropertyGridObject)
    IMPLEMENT_PROPERTY2(PropertyGridObject, Name)
END_IMPLEMENT_PROPERTIES2(PropertyGridObject)

//----------------------------------------------------------------------------------------

PropertyGridObject::PropertyGridObject()
:   mPropertyPointers(Memory::GetPropertyPointerAllocator())
,   mPropertyGridDirty(true)
#if PEGASUS_ENABLE_PROPGRID_SAFE_ACCESSOR
,   mPropertySizes(Memory::GetPropertyPointerAllocator())
#endif
{
    BEGIN_INIT_PROPERTIES(PropertyGridObject)
        INIT_PROPERTY2(Name)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetClassPropertyAccessor(unsigned int index)
{
    if (index < GetNumClassProperties())
    {
        const unsigned int absoluteIndex = GetNumProperties() - GetNumClassProperties() + index;
        return PropertyAccessor(  this
                                , mPropertyPointers[absoluteIndex]
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mPropertySizes[absoluteIndex]
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumClassProperties());
        return PropertyAccessor(  this
                                , nullptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetPropertyAccessor(unsigned int index)
{
    if (index < GetNumProperties())
    {
        return PropertyAccessor(  this
                                , mPropertyPointers[index]
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mPropertySizes[index]
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumProperties());
        return PropertyAccessor(  this
                                , nullptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::AppendPropertyPointer(  void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                               , unsigned int propertySize
#endif
                                               )
{
    PG_ASSERTSTR(ptr != nullptr, "Invalid property pointer added to the PropertyGridObject internal list");
    mPropertyPointers.PushEmpty() = ptr;
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    mPropertySizes.PushEmpty() = propertySize;
#endif
}


}   // namespace PropertyGrid
}   // namespace Pegasus
