/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLib.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLib Class.
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/AssetBuilder.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/AssetRuntimeFactory.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/ByteStream.h"
#include "Pegasus/Allocator/Alloc.h"
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
#include "Pegasus/AssetLib/Category.h"
#endif

namespace Pegasus
{
    extern const Pegasus::PegasusAssetTypeDesc* const* GetAllAssetTypesDescs();
}

using namespace Pegasus;
using namespace Pegasus::AssetLib;

#define PAS_EXTENSION ".pas"

Pegasus::AssetLib::AssetLib::AssetLib(Alloc::IAllocator* allocator, Io::IOManager* mgr)
: mBuilder(allocator),
  mIoMgr(mgr),
  mAllocator(allocator),
  mAssets(allocator),
  mFactories(allocator)
#if PEGASUS_ENABLE_PROXIES
  ,mProxy(this)
#endif
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
  ,mCurrentCategory(nullptr)
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    //instantiate all the asset categories for types
    const Pegasus::PegasusAssetTypeDesc* const* types = Pegasus::GetAllAssetTypesDescs();
    mTypeCategoryCount = 0;
    while (*types != nullptr) { ++types; ++mTypeCategoryCount; }
    types = Pegasus::GetAllAssetTypesDescs(); //reset the list
    mTypeCategories = PG_NEW_ARRAY(mAllocator, -1, "Type categories", Pegasus::Alloc::PG_MEM_PERM, Category, mTypeCategoryCount); 
    for (unsigned i = 0; i < mTypeCategoryCount; ++i)
    {
        mTypeCategories[i].SetUserData(types[i]->mTypeGuid);
    }
#endif
}
char toLow(char c)
{
    return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

char toBrac(char c)
{
    return c == '/' ? '\\' : c;
}

static bool PathsAreEqual(const char* str1, const char* str2)
{
    bool eq = true;
    while (eq && *str1 != '\0' && *str2 != '\0')
    {
        char ch1 = *str1;
        char ch2 = *str2;
        ch1 = toBrac(toLow(ch1));
        ch2 = toBrac(toLow(ch2));
        eq = ch1 == ch2;
        ++str1;
        ++str2;
    }
    return eq && *str1 == *str2;
}

Pegasus::AssetLib::AssetLib::~AssetLib()
{
    for (unsigned int i = 0; i < mAssets.GetSize(); ++i)
    {      
        PG_DELETE(mAllocator, mAssets[i]);
    }

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    PG_DELETE_ARRAY(mAllocator, mTypeCategories);
#endif
}

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
void Pegasus::AssetLib::AssetLib::BeginCategory(Category* category)
{
    mCurrentCategory = category;
}

void Pegasus::AssetLib::AssetLib::EndCategory()
{   
    PG_ASSERTSTR(mCurrentCategory != nullptr, "BeginCategory must be called.");
    bool shouldAddIt = true;
    //push the new category if is not registered already
    for (unsigned i = 0; shouldAddIt && i < mCategories.GetSize(); ++i)
    {
        if (mCategories[i] == mCurrentCategory)
        {
            shouldAddIt = false;
        }
    }

    if (shouldAddIt)
    {
        mCategories.PushEmpty() = mCurrentCategory;
    }

    mCurrentCategory = nullptr;
}

Category* Pegasus::AssetLib::AssetLib::FindTypeCategory(const Pegasus::PegasusAssetTypeDesc* typeDesc)
{
    for (unsigned i = 0; i < mTypeCategoryCount; ++i)
    {
        if (mTypeCategories[i].GetUserData() == typeDesc->mTypeGuid)
        {
            return &mTypeCategories[i];
        }
    }
    PG_FAILSTR("A category was not created for a type! this is a fatal error!");
    return nullptr;
}
#endif

extern void Bison_AssetScriptParse(const Io::FileBuffer* fileBuffer, AssetBuilder* builder);

Io::IoError Pegasus::AssetLib::AssetLib::LoadAsset(const char* path, bool isStructured, Pegasus::AssetLib::Asset** assetOut)
{
    //try to find it first
    for (unsigned int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (PathsAreEqual(mAssets[i]->GetPath(), path))
        {
            if (isStructured != (mAssets[i]->GetFormat() == Pegasus::AssetLib::Asset::FMT_STRUCTURED))
            {
                *assetOut = nullptr;
                return Io::ERR_READING_FILE;
            }
            *assetOut = mAssets[i];
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
            //asset is referenced on this cateogry
            if (mCurrentCategory != nullptr)
            {
                mCurrentCategory->RegisterAsset(*assetOut);
            }
#endif
            return Io::ERR_NONE;
        }
    }

    //not found? lets build it from a file..
    *assetOut = nullptr;
    mBuilder.Reset();
    Io::IoError err = InternalBuildAsset(assetOut, isStructured, path);         
    if (*assetOut != nullptr)
    {
        mAssets.PushEmpty() = *assetOut;
    }
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    if (err == Io::ERR_NONE && mCurrentCategory != nullptr)
    {
        mCurrentCategory->RegisterAsset(*assetOut);
    }
#endif

    return err;
}

Io::IoError Pegasus::AssetLib::AssetLib::ReloadAsset(Pegasus::AssetLib::Asset* assetOut)
{
    PG_ASSERTSTR(assetOut != nullptr, "This asset passed cannot be null!");
    assetOut->Clear();
    return InternalBuildAsset(&assetOut, /*unused*/false, /*unused*/nullptr);   
}

Io::IoError Pegasus::AssetLib::AssetLib::InternalBuildAsset(Asset** assetOut, bool isStructured, const char* path)
{
    mBuilder.Reset();
    bool isPreallocated = *assetOut != nullptr;
    path = isPreallocated ? (*assetOut)->GetPath() : path;
    isStructured = isPreallocated ? (*assetOut)->GetFormat() == Asset::FMT_STRUCTURED : isStructured;

    Io::FileBuffer fileBuffer;
    Io::IoError err = mIoMgr->OpenFileToBuffer(path, fileBuffer, true, mAllocator); //open the raw file first

    if (err == Io::ERR_NONE)
    {
        if (!isPreallocated)
        {
            *assetOut = PG_NEW(mAllocator, -1, "Asset", Alloc::PG_MEM_TEMP) Asset(mAllocator, this, isStructured ? Asset::FMT_STRUCTURED : Asset::FMT_RAW);            
        }
        (*assetOut)->SetPath(path);
        if (isStructured)
        {
            mBuilder.BeginCompilation(*assetOut);
            Bison_AssetScriptParse(&fileBuffer, &mBuilder);    
            if (mBuilder.GetErrorCount() == 0)
            {
                ResolvePendingChildAssets(*assetOut);
            }
            else
            {
                if (!isPreallocated)
                {
                    PG_DELETE(mAllocator, *assetOut);
                    *assetOut = nullptr;
                }
                err = Io::ERR_READING_FILE;
            }
        }
        else
        {
            (*assetOut)->SetFileBuffer(fileBuffer);
            fileBuffer.ForgetBuffer(); //forget this buffer, so we dont destroy it twice.
        }
        
    }
    return err;
}

void Pegasus::AssetLib::AssetLib::UnloadAsset(Asset* asset)
{
    //find this asset
    for (unsigned int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (mAssets[i] == asset)
        {
            if (asset->GetRuntimeData() != nullptr)
            {
                asset->GetRuntimeData()->mAsset = nullptr;
            }
            PG_DELETE(mAllocator, mAssets[i]);
            mAssets.Delete(i);
            return;
        }
    }
    PG_FAILSTR("Asset not found!, do not call this function if this asset is not associated with this library");
}

void Pegasus::AssetLib::AssetLib::ResolvePendingChildAssets(Asset* asset)
{
    //it's important to copy these two vectors. Because the builder gets resetted when loading, this will
    // mean that the builder gets reset when recursive loading takes place.
    Pegasus::Utils::Vector<AssetBuilder::ObjectChildAssetRequest> objectRequest = mBuilder.mObjectChildAssetQueue;
    Pegasus::Utils::Vector<AssetBuilder::ArrayChildAssetRequest> arrayRequests = mBuilder.mArrayChildAssetQueue;

    for (unsigned i = 0; i < objectRequest.GetSize(); ++i)
    {

        AssetBuilder::ObjectChildAssetRequest& request = objectRequest[i];
        RuntimeAssetObjectRef childAsset = LoadObject(request.assetPath);
        if (childAsset != nullptr)
        {
            request.object->AddAsset(request.identifier, childAsset);
        }
    }

    for (unsigned i = 0; i < arrayRequests.GetSize(); ++i)
    {
        AssetBuilder::ArrayChildAssetRequest& request = arrayRequests[i];
        RuntimeAssetObjectRef childAsset = LoadObject(request.assetPath);
        if (childAsset != nullptr)
        {
            Array::Element el;
            el.asset = &(*childAsset);
            request.array->PushElement(el);
        }
    }
}

Asset* Pegasus::AssetLib::AssetLib::CreateAsset(const char* path, bool isStructured)
{
    Asset* asset = nullptr;
    //try to find it first
    for (unsigned int i = 0; i < mAssets.GetSize(); ++i)
    {
        if (PathsAreEqual(mAssets[i]->GetPath(), path))
        {
            PG_LOG('ERR_', "Attempting to create an asset that already exists on cache!");
            return nullptr;  //Cant allow to override this asset
        }
    }

    // structured means its a json file. non structured means it does not get parsed and the file gets raw'd
    asset = PG_NEW(mAllocator, -1, "Asset", Alloc::PG_MEM_TEMP) Asset(mAllocator, this, isStructured ? Asset::FMT_STRUCTURED : Asset::FMT_RAW);
    asset->SetPath(path);
    mAssets.PushEmpty() = asset;

    if (!isStructured)
    {
        //alocate a little fake buffer
        Io::FileBuffer fileBuffer;
        char* buff = PG_NEW_ARRAY(mAllocator, -1, "Raw Asset", Alloc::PG_MEM_TEMP, char, 1);
        buff[0] = '\0';
        fileBuffer.OwnBuffer(mAllocator, buff, 1);
        asset->SetFileBuffer(fileBuffer);
        fileBuffer.ForgetBuffer();
    }

    return asset;
}

Io::IoError Pegasus::AssetLib::AssetLib::SaveAsset(Asset* asset)
{
    if (asset->GetFormat() == Asset::FMT_STRUCTURED)
    {
        Utils::ByteStream bs(mAllocator);
        asset->DumpToStream(bs);
        Io::FileBuffer fb;
        fb.OwnBuffer(mAllocator, static_cast<char*>(bs.GetBuffer()), bs.GetSize());
        Io::IoError result = mIoMgr->SaveFileToBuffer(asset->GetPath(), fb);
        fb.ForgetBuffer();
        return result;
    }
    else
    {
        return mIoMgr->SaveFileToBuffer(asset->GetPath(), *asset->Raw());
    }
}

void Pegasus::AssetLib::AssetLib::RegisterObjectFactory(AssetRuntimeFactory* factory)
{
#if PEGASUS_DEBUG
    //check if there is a duplicate factory, assert if so.
    for (unsigned int i = 0; i < mFactories.GetSize(); ++i)
    {
        PG_ASSERTSTR(mFactories[i] != factory, "factory must not be inserted twice!");
    }
#endif

    factory->SetAssetLib(this);

    mFactories.PushEmpty() = factory;
}

RuntimeAssetObjectRef Pegasus::AssetLib::AssetLib::LoadObject(const char* path)
{
    
    Pegasus::AssetLib::Asset* asset = nullptr;

    bool isStructured = true; //assume is structured.

    const char* extension = Utils::Strrchr(path, '.');
    if (extension == nullptr || extension[0] == '\0')
    {
        PG_LOG('ERR_', "Extension is required for any asset loaded through the asset system.");
        return nullptr;
    }

    ++extension; //skip the . character

    //find out if its structured or not.
    const Pegasus::PegasusAssetTypeDesc* const* desc = GetAllAssetTypesDescs();
    while (*desc != nullptr)
    {
        if (!Utils::Stricmp(extension,(*desc)->mExtension))
        {
            isStructured = (*desc)->mIsStructured;
            break;
        }
        ++desc;
    }
    
    if (Io::ERR_NONE == LoadAsset(path, isStructured, &asset))
    {
        PG_ASSERT(asset != nullptr);
        //Has this object been created already? if so return it.
        if (asset->GetRuntimeData() != nullptr)
        {
            return asset->GetRuntimeData();
        }
        const Pegasus::PegasusAssetTypeDesc* foundDesc = nullptr;    
        Pegasus::AssetLib::AssetRuntimeFactory* factory = FindFactory(asset, extension, &foundDesc);
        if (factory == nullptr)
        {
            PG_LOG('ERR_', "Unable to identify factory for asset %s", path);
            UnloadAsset(asset);
        }
        else
        {
            RuntimeAssetObjectRef obj = factory->CreateRuntimeObject(foundDesc);
            PG_ASSERTSTR(obj != nullptr, "FATAL: factory returned invalid asset runtime object.");
            asset->SetTypeDesc(foundDesc);
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
            FindTypeCategory(foundDesc)->RegisterAsset(asset);
#endif
            if (obj->Read(asset)) //Read binds this asset together.
            {
                return obj;
            }
            else
            {
                UnloadAsset(asset);
                PG_LOG('ERR_', "Error parsing asset %s correctly.", path);
            }
        }
    }
    else
    {
        PG_LOG('ERR_', "Unable to load asset %s correctly.", path);
    }

    return nullptr;
}

RuntimeAssetObjectRef Pegasus::AssetLib::AssetLib::CreateObject(const char* path, const PegasusAssetTypeDesc* desc)
{
    Asset* asset = CreateAsset(path, desc->mIsStructured);
    if (asset != nullptr)
    {
        AssetRuntimeFactory* factory = nullptr;
        const PegasusAssetTypeDesc* supportedType = nullptr;
        //find an available factory
        for (unsigned int i = 0; factory == nullptr &&  i < mFactories.GetSize(); ++i)
        {
            AssetRuntimeFactory* candidate = mFactories[i];
            const PegasusAssetTypeDesc* const* supportedTypeList = candidate->GetAssetTypes();
            while (supportedTypeList != nullptr && *supportedTypeList != nullptr)
            {
                if (
                    (*supportedTypeList)->mTypeGuid == desc->mTypeGuid
                )
                {
                    supportedType = *supportedTypeList;
                    factory = candidate;
                    break;
                }
                ++supportedTypeList;
            }
        }

        if (factory == nullptr)
        {
            PG_LOG('ERR_', "Unable to create object. No factory found for %s.", path);
            UnloadAsset(asset);
        }
        else
        {
            RuntimeAssetObjectRef obj = factory->CreateRuntimeObject(desc);
            asset->SetTypeDesc(desc);
            obj->Bind(asset);
            PEGASUS_EVENT_DISPATCH(this, RuntimeAssetObjectCreated, obj->GetProxy());
            return obj;
        }
    }
    
    return nullptr;
}

Io::IoError Pegasus::AssetLib::AssetLib::SaveObject(RuntimeAssetObjectRef object)
{
    Asset* asset = object->GetOwnerAsset();
    if (asset == nullptr)
    {
        PG_LOG('ERR_', "Object has no asset attached to it.");
        return Io::ERR_WRITING_FILE;
    }

    object->Write(asset);

    return SaveAsset(asset);

}

Pegasus::AssetLib::AssetRuntimeFactory* Pegasus::AssetLib::AssetLib::FindFactory(Asset* asset, const char* ext, const PegasusAssetTypeDesc** outDesc) const
{
    for (unsigned int i = 0; i < mFactories.GetSize(); ++i)
    {
        if (mFactories[i]->IsCompatible(asset,ext,outDesc))
        {
            return mFactories[i];
        }
    }
    *outDesc = nullptr;
    return nullptr;
}
