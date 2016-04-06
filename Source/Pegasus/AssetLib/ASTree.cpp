/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ASTree.h
//! \author Kleber Garcia
//! \date   8th February 2015
//! \brief  Asset Script Tree Implementation

#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Shared/AssetEvent.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

#define GenericFindElement(list, name) \
    int sz = list.GetSize(); \
    for (int i = 0; i < sz; ++i) { if (!Utils::Strcmp(name, list[i].mName)) { return i; } }\
    return -1;

Object::Object(Alloc::IAllocator* alloc)
: mInts(alloc),
  mFloats(alloc),
  mStrings(alloc),
  mObjects(alloc),
  mAssets(alloc),
  mArrays(alloc)
#if PEGASUS_ENABLE_PROXIES
  ,mProxy(this)
#endif
{
}

Object::~Object()
{
}

int         Object::GetInt(int i) const
{
    return mInts[i].mEl;
}

int         Object::FindInt(const char* name) const
{
    GenericFindElement(mInts, name);
}

const char* Object::GetIntName(int i) const
{
    return mInts[i].mName;
}

void        Object::AddInt(const char* name, int i)
{
    Touple<int>& t = mInts.PushEmpty();
    t.mName = name;
    t.mEl = i;
}


float       Object::GetFloat(int i) const
{
    return  mFloats[i].mEl; 
}

int         Object::FindFloat(const char* name) const
{
    GenericFindElement(mFloats, name);
}

const char* Object::GetFloatName(int i) const
{
    return mFloats[i].mName;
}

void        Object::AddFloat(const char* name, float f)
{
    Touple<float>& t = mFloats.PushEmpty();
    t.mEl = f;
    t.mName = name;
}


const char* Object::GetString(int i) const
{
    return mStrings[i].mEl;
}

int         Object::FindString(const char* name) const
{
    GenericFindElement(mStrings, name);
}

const char* Object::GetStringName(int i) const
{
    return mStrings[i].mName;
}

void        Object::AddString(const char* name, const char* str)
{
    Touple<const char*>& t = mStrings.PushEmpty();
    t.mEl = str;
    t.mName = name;
}


Object*     Object::GetObject(int i) const
{
    return mObjects[i].mEl;
}

int         Object::FindObject(const char* name) const
{
    GenericFindElement(mObjects, name);
}

const char* Object::GetObjectName(int i) const
{
    return mObjects[i].mName;
}

void        Object::AddObject(const char* name, Object* o)
{
    Touple<Object*>& t = mObjects.PushEmpty();
    t.mEl = o;
    t.mName = name;

}

RuntimeAssetObjectRef  Object::GetAsset(int i) const
{
    return mAssets[i].mEl;
}

void Object::AddAsset(const char* name, RuntimeAssetObjectRef obj)
{
    Touple<RuntimeAssetObjectRef>& t =  mAssets.PushEmpty();
    t.mEl = obj;
    t.mName = name;
    PEGASUS_EVENT_DISPATCH(obj->GetOwnerAsset()->GetLib(), AssetLinkAdded, GetProxy(), obj->GetOwnerAsset()->GetProxy());
}

int Object::FindAsset(const char* name) const
{
    GenericFindElement(mAssets, name);
}

const char* Object::GetAssetName(int i) const
{
    return mAssets[i].mName;
}

Array*      Object::GetArray(int i) const
{
    return mArrays[i].mEl;
}

int         Object::FindArray(const char* name) const
{
    GenericFindElement(mArrays, name);
}

const char* Object::GetArrayName(int i) const
{
    return mArrays[i].mName;
}

void        Object::AddArray(const char* name, Array* arr)
{
    Touple<Array*>& t =  mArrays.PushEmpty();
    t.mEl = arr;
    t.mName = name;
}

Array::Array(Alloc::IAllocator* alloc)
:   mType(Array::AS_TYPE_NULL),
    mElements(alloc),
    mAssetObjReferences(alloc)
#if PEGASUS_ENABLE_PROXIES
    ,mProxy(this)
#endif
{
}

Array::~Array()
{
}

void Array::CommitType(Type t)
{
    PG_ASSERT(mType == Array::AS_TYPE_NULL);
    mType = t;
}

const Array::Element& Array::GetElement(int i) const
{
    return mElements[i];
}

void     Array::PushElement(Array::Element& el)
{
    mElements.PushEmpty() = el;
    if (mType == Array::AS_TYPE_ASSET_PATH_REF)
    {
        mAssetObjReferences.PushEmpty() = el.asset;
    }
}
