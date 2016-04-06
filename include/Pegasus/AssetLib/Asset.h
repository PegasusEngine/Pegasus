/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Asset.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  Asset Class.

#ifndef PEGASUS_ASSET_H
#define PEGASUS_ASSET_H

#define MAX_ASSET_PATH_STRING 512

#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Memory/BlockAllocator.h"
#include "Pegasus/Utils/Vector.h"

//Fwd declarations
namespace Pegasus
{
    struct PegasusAssetTypeDesc;

    namespace Utils {
        class ByteStream;
    }

    namespace Alloc {   
        class IAllocator;
    }

}

namespace Pegasus
{
namespace AssetLib
{

class Object;
class Array;
class RuntimeAssetObject;
class AssetLib;
class RuntimeAssetObjectProxy;

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
class Category;
#endif

//! Asset representation
class Asset
{
    friend RuntimeAssetObjectProxy;
    friend RuntimeAssetObject;
    friend AssetLib;
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    friend class Category;
#endif

public:

    enum AssetFormat
    {
        FMT_RAW,
        FMT_STRUCTURED
    };
    
    //! Constructor for structured asset
    //! \param obj the root object used for this asset.
    Asset(Alloc::IAllocator* allocator, Pegasus::AssetLib::AssetLib* lib, AssetFormat fmt);

    //! Destructor
    ~Asset();

    //! Sets the root object
    void SetRootObject(Object* obj);

    //! Sets the file buffer if this is a raw asset
    void SetFileBuffer(const Io::FileBuffer& fb);

    //! Gets the root object
    //! \return the root object
    Object* Root() { PG_ASSERT(mFormat == FMT_STRUCTURED); return mRoot; }

    //! Gets the root object
    //! \return the root object
    const Object* Root() const { PG_ASSERT(mFormat == FMT_STRUCTURED); return mRoot; }

    //! Gets the raw buffer of this object
    //! \return the raw buffer of this asset
    Io::FileBuffer* Raw() { PG_ASSERT(mFormat == FMT_RAW); return &mRawAsset; }

    //! Sets the path of this assets file
    //! \param the full path of this asset
    void SetPath(const char* path);

    //! Gets the path of this assets file
    //! \return the full path of this asset
    const char* GetPath() const { return mPathString; }

    //! dumps this asset (writes it) to a byte stream
    void DumpToStream(Utils::ByteStream& bs);

    //! Gets the format of this asset
    AssetFormat GetFormat() const { return mFormat; }

    //! Creates a new object whose lifetime is this assets object
    //! \return the object
    Object* NewObject();

    //! Creates a new array whose lifetime is this assets object
    //! \return the array
    Array* NewArray();

    //! Returns the owner asset library
    //! \return the asset library
    Pegasus::AssetLib::AssetLib* GetLib() const { return mAssetLib; }

    //! Copies a string passed in, whose lifetime is this assets object
    const char* CopyString(const char* string);

    //! Resets this asset internally (destroys all internal memory and makes it an empty asset)
    void Clear();

    //! Gets the runtime data
    RuntimeAssetObject* GetRuntimeData() const { return mRuntimeData; }

    //! Gets the type description of this asset
    //! \return the type description. Null if this is a raw json or binary file.
    const PegasusAssetTypeDesc* GetTypeDesc() const { return mTypeDesc; }

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    Utils::Vector<Category*>& GetCategories() { return mCategories; }
#endif

#if PEGASUS_ENABLE_PROXIES
    IAssetProxy* GetProxy() { return &mProxy; }
    const IAssetProxy* GetProxy() const { return &mProxy; }

    //! \return the file name of this asset.
    const char * GetName() const { return mName; }
#endif

private:

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    //! Registers a category
    void RegisterToCategory(Category* category);
    //! Unregisters a category
    void UnregisterToCategory(Category* category);
#endif

    //! Sets the runtime data
    void SetRuntimeData(RuntimeAssetObject * obj) { mRuntimeData = obj; }

    //! Sets the type desc
    //! \param desc the type desc
    void SetTypeDesc(const PegasusAssetTypeDesc* desc) { mTypeDesc = desc; }

    Alloc::IAllocator* mAllocator;
    RuntimeAssetObject*    mRuntimeData;
    Memory::BlockAllocator mAstAllocator;
    Memory::BlockAllocator mStringAllocator;

    Utils::Vector<Array*>  mChildArrays;
    Utils::Vector<Object*> mChildObjects;

    Object* mRoot;
    Io::FileBuffer mRawAsset;
    char mPathString[MAX_ASSET_PATH_STRING];
    AssetFormat mFormat;

    Pegasus::AssetLib::AssetLib* mAssetLib;

    const Pegasus::PegasusAssetTypeDesc* mTypeDesc;

#if PEGASUS_ENABLE_PROXIES
    //! Filename metadata. Name of this asset.
    char mName[MAX_ASSET_PATH_STRING];
    AssetProxy mProxy;
#endif

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    Utils::Vector<Category*> mCategories;
#endif
};

}
}

#endif
