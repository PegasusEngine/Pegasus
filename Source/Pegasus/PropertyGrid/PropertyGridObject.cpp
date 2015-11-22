/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObject.cpp
//! \author	Karolyn Boulanger
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

    //! \todo Use a fast memcpy function that always take the fast path
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

    //! \todo Use a fast memcpy function that always take the fast path
    Utils::Memcpy(mPtr, inputBuffer, inputBufferSize);
    mObj->InvalidatePropertyGrid();
}

//----------------------------------------------------------------------------------------

BEGIN_IMPLEMENT_PROPERTIES(PropertyGridObject)
    IMPLEMENT_PROPERTY(PropertyGridObject, Name)
END_IMPLEMENT_PROPERTIES(PropertyGridObject)

//----------------------------------------------------------------------------------------

PropertyGridObject::PropertyGridObject()
:   mClassPropertyPointers(Memory::GetPropertyPointerAllocator())
,   mPropertyGridDirty(true)
#if PEGASUS_ENABLE_PROPGRID_SAFE_ACCESSOR
,   mClassPropertySizes(Memory::GetPropertyPointerAllocator())
#endif
,   mObjectProperties(Memory::GetPropertyPointerAllocator())
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    BEGIN_INIT_PROPERTIES(PropertyGridObject)
        INIT_PROPERTY(Name)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

PropertyGridObject::~PropertyGridObject()
{
    ClearObjectProperties();
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetDerivedClassPropertyAccessor(unsigned int index)
{
    if (index < GetNumDerivedClassProperties())
    {
        const unsigned int absoluteIndex = GetNumClassProperties() - GetNumDerivedClassProperties() + index;
        return PropertyAccessor(  this
                                , mClassPropertyPointers[absoluteIndex]
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mClassPropertySizes[absoluteIndex]
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumDerivedClassProperties());
        return PropertyAccessor(  this
                                , nullptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetClassPropertyAccessor(unsigned int index)
{
    if (index < GetNumClassProperties())
    {
        return PropertyAccessor(  this
                                , mClassPropertyPointers[index]
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mClassPropertySizes[index]
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access class property %u but it has to be < %u", index, GetNumClassProperties());
        return PropertyAccessor(  this
                                , nullptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::AddObjectProperty(PropertyType type, int typeSize, const char * name, const char * typeName, const void * defaultValuePtr)
{
    PG_ASSERTSTR(name != nullptr, "Trying to create an object property without name");
    PG_ASSERTSTR(type < NUM_PROPERTY_TYPES, "Trying to create the object property \"%s\" with an invalid type (%i)", name, type);
    PG_ASSERTSTR(typeSize > 0, "Trying to create the object property \"%s\" with an invalid type size (%i)", name, typeSize);
    PG_ASSERTSTR(typeName != nullptr, "Trying to create the object property \"%s\" without type name", name);
    PG_ASSERTSTR(defaultValuePtr != nullptr, "Trying to create the object property \"%s\" without default value", name);

    // Create the record of the object property
    ObjectProperty & prop = mObjectProperties.PushEmpty();
    prop.record.type = type;
    prop.record.size = typeSize;
    prop.record.name = name;
    prop.record.typeName = typeName;

    // Allocate the memory for the default value of the object property then copy the value
    prop.record.defaultValuePtr = PG_NEW_ARRAY(Memory::GetPropertyPointerAllocator(), -1, "ObjectPropertyDefault", Pegasus::Alloc::PG_MEM_PERM, unsigned char, typeSize);
    Utils::Memcpy(prop.record.defaultValuePtr, defaultValuePtr, typeSize);

    // Allocate the memory for the object property then copy the value
    prop.valuePtr = PG_NEW_ARRAY(Memory::GetPropertyPointerAllocator(), -1, "ObjectProperty", Pegasus::Alloc::PG_MEM_PERM, unsigned char, typeSize);
    Utils::Memcpy(prop.valuePtr, defaultValuePtr, typeSize);
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::RemoveObjectProperty(unsigned int index)
{
    if (index < mObjectProperties.GetSize())
    {
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[index].record.defaultValuePtr);
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[index].valuePtr);
        mObjectProperties.Delete(index);
    }
    else
    {
        PG_FAILSTR("Trying to remove the object property %u but only %u properties are defined", index, mObjectProperties.GetSize());
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::ClearObjectProperties()
{
    for (unsigned int p = 0; p < mObjectProperties.GetSize(); ++p)
    {
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[p].record.defaultValuePtr);
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[p].valuePtr);
    }
    mObjectProperties.Clear();
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetObjectPropertyAccessor(unsigned int index)
{
    if (index < GetNumObjectProperties())
    {
        return PropertyAccessor(  this
                                , mObjectProperties[index].valuePtr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mObjectProperties[index].record.size
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access object property %u but it has to be < %u", index, GetNumObjectProperties());
        return PropertyAccessor(  this
                                , nullptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::AppendClassPropertyPointer(  void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                                    , unsigned int propertySize
#endif
                                                    )
{
    PG_ASSERTSTR(ptr != nullptr, "Invalid property pointer added to the PropertyGridObject internal list");
    mClassPropertyPointers.PushEmpty() = ptr;
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    mClassPropertySizes.PushEmpty() = propertySize;
#endif
}


}   // namespace PropertyGrid
}   // namespace Pegasus
