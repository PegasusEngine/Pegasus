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
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"

using namespace Pegasus::AssetLib;

namespace Pegasus {
namespace PropertyGrid {


void PropertyReadAccessor::Read(void * outputBuffer, unsigned int outputBufferSize) const
{
    PG_ASSERTSTR(outputBuffer != nullptr, "Trying to read a property into a null buffer.");
    PG_ASSERTSTR(outputBufferSize > 0, "Trying to read a property into a buffer with an undefined size.");
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    PG_ASSERTSTR(outputBufferSize == mSize, "Trying to read a property into a buffer whose size is incorrect.");
#endif

    //! \todo Use a fast memcpy function that always take the fast path
    Utils::Memcpy(outputBuffer, mConstPtr, outputBufferSize);
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

    PEGASUS_EVENT_DISPATCH(mObj, ValueChangedEventIndexed, mCategory, mIndex);

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
    PEGASUS_EVENT_INIT_DISPATCHER

    BEGIN_INIT_PROPERTIES(PropertyGridObject)
        INIT_PROPERTY(Name)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

PropertyGridObject::~PropertyGridObject()
{
    ClearObjectProperties();
    PEGASUS_EVENT_DISPATCH(this, PropertyGridDestroyed);
}

//----------------------------------------------------------------------------------------

PropertyAccessor PropertyGridObject::GetDerivedClassPropertyAccessor(unsigned int index)
{
    if (index < GetNumDerivedClassProperties())
    {
        const unsigned int absoluteIndex = GetNumClassProperties() - GetNumDerivedClassProperties() + index;
        return PropertyAccessor(  this
                                , mClassPropertyPointers[absoluteIndex]
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_CLASS
                                ,  absoluteIndex
#endif
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
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_INVALID
                                ,  -1
#endif
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
                                ,  mClassPropertyPointers[index]
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_CLASS
                                ,  index
#endif
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
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_INVALID
                                ,  -1
#endif
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

const PropertyReadAccessor PropertyGridObject::GetClassReadPropertyAccessor(unsigned int index) const
{
    if (index < GetNumClassProperties())
    {
        return PropertyReadAccessor(  this
                                , mClassPropertyPointers[index]
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_CLASS
                                ,  index
#endif
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , mClassPropertySizes[index]
#endif
                               );
    }
    else
    {
        PG_FAILSTR("Trying to access property %u but it has to be < %u", index, GetNumClassProperties());
        return PropertyReadAccessor(  this
                                , nullptr
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_INVALID
                                , -1 
#endif
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::AddObjectProperty(PropertyType type, int typeSize, const char * name, const char * typeName, const void * defaultValuePtr, const EditorDesc* editorDesc)
{
    PG_ASSERTSTR(name != nullptr, "Trying to create an object property without name");
    PG_ASSERTSTR(type < NUM_PROPERTY_TYPES, "Trying to create the object property \"%s\" with an invalid type (%i)", name, type);
    PG_ASSERTSTR(typeSize > 0, "Trying to create the object property \"%s\" with an invalid type size (%i)", name, typeSize);
    PG_ASSERTSTR(typeName != nullptr, "Trying to create the object property \"%s\" without type name", name);
    PG_ASSERTSTR(defaultValuePtr != nullptr, "Trying to create the object property \"%s\" without default value", name);

    // Create the record of the object property
    ObjectProperty & prop = mObjectProperties.PushEmpty();
    prop.record.category = PROPERTYCATEGORY_OBJECT;
    prop.record.type = type;
    prop.record.size = typeSize;
    prop.record.typeName = typeName;

    if (editorDesc == nullptr)
    {
        EditorDesc& defaultEditorDesc = prop.record.editorDesc;
        defaultEditorDesc.editorType = EDITOR_DEFAULT;
        Utils::Memset8(&defaultEditorDesc.params, 0, sizeof(defaultEditorDesc.params));
    }
    else
    {
        prop.record.editorDesc = *editorDesc;
    }

    // Allocate the memory for the default value of the object property then copy the value
    prop.record.defaultValuePtr = PG_NEW_ARRAY(Memory::GetPropertyPointerAllocator(), -1, "ObjectPropertyDefault", Pegasus::Alloc::PG_MEM_PERM, unsigned char, typeSize);
    Utils::Memcpy(prop.record.defaultValuePtr, defaultValuePtr, typeSize);

    
    //Allocate memory to copy the name passed
    char* copiedName = PG_NEW_ARRAY(Memory::GetPropertyPointerAllocator(), -1, "CopiedName", Pegasus::Alloc::PG_MEM_PERM, char, Utils::Strlen(name) + 1);
    copiedName[0] = '\0';
    Utils::Strcat(copiedName, name);
    prop.record.name = copiedName;

    // Allocate the memory for the object property then copy the value
    prop.valuePtr = PG_NEW_ARRAY(Memory::GetPropertyPointerAllocator(), -1, "ObjectProperty", Pegasus::Alloc::PG_MEM_PERM, unsigned char, typeSize);
    Utils::Memcpy(prop.valuePtr, defaultValuePtr, typeSize);
    
    //notify the editor that the layout of this object has changed
    PEGASUS_EVENT_DISPATCH(this, ObjectPropertiesLayoutChanged);
}

//----------------------------------------------------------------------------------------

void PropertyGridObject::RemoveObjectProperty(unsigned int index)
{
    if (index < mObjectProperties.GetSize())
    {
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[index].record.defaultValuePtr);
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[index].record.name);
        PG_DELETE_ARRAY(Memory::GetPropertyPointerAllocator(), mObjectProperties[index].valuePtr);
        mObjectProperties.Delete(index);

        //notify the editor that the layout of this object has changed
        PEGASUS_EVENT_DISPATCH(this, ObjectPropertiesLayoutChanged);
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
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_OBJECT
                                ,  index
#endif
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
#if PEGASUS_USE_EVENTS
                                ,  PROPERTYCATEGORY_INVALID
                                , -1 
#endif
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                , 0
#endif
                               );
    }
}

//----------------------------------------------------------------------------------------

int PropertyGridObject::AppendClassPropertyPointer(  void * ptr
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
                                                    , unsigned int propertySize
#endif
                                                    )
{
    int index = mClassPropertyPointers.GetSize();
    PG_ASSERTSTR(ptr != nullptr, "Invalid property pointer added to the PropertyGridObject internal list");
    mClassPropertyPointers.PushEmpty() = ptr;
#if PEGASUS_ENABLE_PROPERTYGRID_SAFE_ACCESSOR
    mClassPropertySizes.PushEmpty() = propertySize;
#endif
    return index;
}

//----------------------------------------------------------------------------------------

template <class V, int D>
static void WriteVector(AssetLib::Object* obj, V v, AssetLib::Asset* asset, const char* name)
{
    AssetLib::Array* arr = asset->NewArray();
    obj->AddArray(name, arr);
    arr->CommitType(AssetLib::Array::AS_TYPE_FLOAT);
    
    for (int i = 0; i < D; ++i)
    {
        AssetLib::Array::Element e;
        e.f = v.v[i];
        arr->PushElement(e);
    }

}

//----------------------------------------------------------------------------------------

template <class C, int D>
static void WriteColor(AssetLib::Object* obj, C c, AssetLib::Asset* asset, const char* name)
{
    AssetLib::Array* arr = asset->NewArray();
    obj->AddArray(name, arr);
    arr->CommitType(AssetLib::Array::AS_TYPE_INT);
    
    for (int i = 0; i < D; ++i)
    {
        AssetLib::Array::Element e;
        e.i = static_cast<int>(c.v[i]);
        arr->PushElement(e);
    }

}

//----------------------------------------------------------------------------------------

void PropertyGridObject::WriteToObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj) const
{
    for (unsigned int i = 0; i < GetNumClassProperties(); ++i)
    {
        const PropertyRecord& r = GetClassPropertyRecord(i);
        const PropertyReadAccessor a = GetClassReadPropertyAccessor(i);
        switch(r.type)
        {
        case PROPERTYTYPE_BOOL:
            {
                obj->AddInt(r.name, a.Get<bool>());
            }
            break;
        case PROPERTYTYPE_UINT:
        case PROPERTYTYPE_INT:
            {
                obj->AddInt(r.name, a.Get<int>());
            }
            break;
        case PROPERTYTYPE_FLOAT:
            {
                obj->AddFloat(r.name, a.Get<float>());
            }
            break;
        case PROPERTYTYPE_VEC2:
            {
                WriteVector<Math::Vec2,2>(obj, a.Get<Math::Vec2>(), parentAsset, r.name);
            }
            break;
        case PROPERTYTYPE_VEC3:
            {
                WriteVector<Math::Vec3,3>(obj, a.Get<Math::Vec3>(), parentAsset, r.name);
            }
            break;
        case PROPERTYTYPE_VEC4:
            {
                WriteVector<Math::Vec4,4>(obj, a.Get<Math::Vec4>(), parentAsset, r.name);
            }
            break;
        case PROPERTYTYPE_COLOR8RGB:
            {
                WriteColor<Math::Color8RGB, 3>(obj, a.Get<Math::Color8RGB>(), parentAsset, r.name);
            }
            break;
        case PROPERTYTYPE_COLOR8RGBA:
            {
                WriteColor<Math::Color8RGBA, 4>(obj, a.Get<Math::Color8RGBA>(), parentAsset, r.name);
            }
            break;
        case PROPERTYTYPE_STRING64:
            {
                char str64[64];
                a.Read(str64, sizeof(str64));
                obj->AddString(r.name, parentAsset->CopyString(str64));
            }
            break;
        }
    }
}

//----------------------------------------------------------------------------------------

template<class C, int D>
bool ReadVector(PropertyAccessor& a, const char* name, AssetLib::Object* object)
{
    int index = object->FindArray(name);
    if (index == -1)
    {
        return false;
    }

    AssetLib::Array* arr = object->GetArray(index);
    if (arr->GetType() != AssetLib::Array::AS_TYPE_FLOAT && arr->GetSize() != D)
    {
        return false;
    }

    C c;
    for (int i = 0; i < D; ++i)
    {
        c.v[i] = arr->GetElement(i).f;
    }

    a.Set<C>(c);
    return true;
}

//----------------------------------------------------------------------------------------

template<class C, int D>
bool ReadColor(PropertyAccessor& a, const char* name, AssetLib::Object* object)
{
    int index = object->FindArray(name);
    if (index == -1)
    {
        return false;
    }

    AssetLib::Array* arr = object->GetArray(index);
    if (arr->GetType() != AssetLib::Array::AS_TYPE_INT && arr->GetSize() != D)
    {
        return false;
    }

    C c;
    for (int i = 0; i < D; ++i)
    {
        c.v[i] = arr->GetElement(i).i;
    }

    a.Set<C>(c);
    return true;
}


//----------------------------------------------------------------------------------------

bool PropertyGridObject::ReadFromObject(AssetLib::Asset* parentAsset, AssetLib::Object* obj)
{
    for (unsigned int i = 0; i < GetNumClassProperties(); ++i)
    {
        const PropertyRecord& r = GetClassPropertyRecord(i);
        PropertyAccessor a = GetClassPropertyAccessor(i);
        switch(r.type)
        {
        case PROPERTYTYPE_BOOL:
            {
                int index = obj->FindInt(r.name);
                if (index == -1) continue;
                a.Set<bool>(obj->GetInt(index) != 0);
            }
            break;
        case PROPERTYTYPE_UINT:
            {
                int index = obj->FindInt(r.name);
                if (index == -1) continue;
                a.Set<unsigned int>(static_cast<unsigned int>(obj->GetInt(index)));
            }
        case PROPERTYTYPE_INT:
            {
                int index = obj->FindInt(r.name);
                if (index == -1) continue;
                a.Set<int>(obj->GetInt(index));
            }
            break;
        case PROPERTYTYPE_FLOAT:
            {
                int index = obj->FindFloat(r.name);
                if (index == -1) continue;
                a.Set<float>(obj->GetFloat(index));
            }
            break;
        case PROPERTYTYPE_VEC2:
            {
                if (!ReadVector<Math::Vec2, 2>(a, r.name, obj)) continue;
            }
            break;
        case PROPERTYTYPE_VEC3:
            {
                if (!ReadVector<Math::Vec3, 3>(a, r.name, obj)) continue;
            }
            break;
        case PROPERTYTYPE_VEC4:
            {
                if (!ReadVector<Math::Vec4, 4>(a, r.name, obj)) continue;
            }
            break;
        case PROPERTYTYPE_COLOR8RGB:
            {
                if (!ReadColor<Math::Color8RGB, 3>(a, r.name, obj)) continue;
            }
            break;
        case PROPERTYTYPE_COLOR8RGBA:
            {
                if (!ReadColor<Math::Color8RGBA, 4>(a, r.name, obj)) continue;
            }
            break;
        case PROPERTYTYPE_STRING64:
            {
                int index = obj->FindString(r.name);
                if (index == -1) continue;
                const char* str = obj->GetString(index); 
                int len = Utils::Strlen(str);
                if (len < 64)
                {
                    char tempString[64];
                    Utils::Memcpy(tempString, str, len + 1);
                    a.Write(str, 64);
                }
            }
            break;
        }
    }

    return true;
}


}   // namespace PropertyGrid
}   // namespace Pegasus
