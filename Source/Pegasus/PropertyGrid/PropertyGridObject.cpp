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

namespace Pegasus {
namespace PropertyGrid {


BEGIN_IMPLEMENT_PROPERTIES2(PropertyGridObject)
    IMPLEMENT_PROPERTY2(PropertyGridObject, Name)
END_IMPLEMENT_PROPERTIES2(PropertyGridObject)

//----------------------------------------------------------------------------------------

PropertyGridObject::PropertyGridObject()
:   mPropertyPointers(Memory::GetPropertyPointerAllocator())
,   mPropertyGridDirty(true)
{
    BEGIN_INIT_PROPERTIES(PropertyGridObject)
        INIT_PROPERTY2(Name)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

PropertyReader PropertyGridObject::GetClassPropertyReader(unsigned int index) const
{
    if (index < GetNumClassProperties())
    {
        return PropertyReader(mPropertyPointers[GetNumProperties() - GetNumClassProperties() + index]);
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumClassProperties());
        return PropertyReader(nullptr);
    }
}

//----------------------------------------------------------------------------------------

PropertyWriter PropertyGridObject::GetClassPropertyWriter(unsigned int index)
{
    if (index < GetNumClassProperties())
    {
        return PropertyWriter(this, mPropertyPointers[GetNumProperties() - GetNumClassProperties() + index]);
    }
    else
    {
        PG_FAILSTR("Trying to access class property %u but it has to be < %u", index, GetNumClassProperties());
        return PropertyWriter(this, nullptr);
    }
}

//----------------------------------------------------------------------------------------

PropertyReader PropertyGridObject::GetPropertyReader(unsigned int index) const
{
    if (index < GetNumProperties())
    {
        return PropertyReader(mPropertyPointers[index]);
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumProperties());
        return PropertyReader(nullptr);
    }
}

//----------------------------------------------------------------------------------------

PropertyWriter PropertyGridObject::GetPropertyWriter(unsigned int index)
{
    if (index < GetNumProperties())
    {
        return PropertyWriter(this, mPropertyPointers[index]);
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumProperties());
        return PropertyWriter(this, nullptr);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::AppendPropertyPointer(void * ptr)
{
    PG_ASSERTSTR(ptr != nullptr, "Invalid property pointer added to the PropertyGridObject internal list");
    mPropertyPointers.PushEmpty() = ptr;
}


}   // namespace PropertyGrid
}   // namespace Pegasus
