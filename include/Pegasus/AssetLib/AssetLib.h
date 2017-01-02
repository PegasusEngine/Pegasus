/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLib.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLib Class.

#ifndef PEGASUS_ASSET_LIB_H
#define PEGASUS_ASSET_LIB_H

#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/AssetBuilder.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/AssetLib/Shared/AssetEvent.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/AssetLib/Proxy/AssetLibProxy.h"
#endif

//FWD declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace AssetLib
{
class Asset;
class RuntimeAssetObject;
class AssetRuntimeFactory;
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
class Category;
#endif
//! Asset Library class
class AssetLib
{
public:

    PEGASUS_EVENT_DECLARE_DISPATCHER(IAssetEventListener)
    void InvalidateData() { /*for compliance, when SetEventListener is called */ }
    //! Constructor
    //! \param allocator the allocator to use internally
    //! \param ioMgr the input/output mgr, containing logic for the file system
    AssetLib(Alloc::IAllocator* allocator, Io::IOManager* ioMgr);

    //! Destructor
    ~AssetLib();

    //---- Low Level functions, use these if we want to just use / save / modify pure json ----------//

    //! Loads an asset. An asset is a database fragment with properties (strings / ints / floats and arrays).
    //! It is the runtime representation of a json file (if structured). 
    //! This function only opens the asset file, parses it, caches it if necessary.
    //! \param the file path used.
    //! \param true if the file underneath is a json file. False if this asset is raw (a raw byte buffer).
    //! \param the output asset pointer. This gets set to null if the return value != ERR_NONE
    //! \return the IO error. If successful, we are guaranteed to fill in the output pointer.
    Io::IoError LoadAsset(const char* path, bool isStructured, Asset** asset);

    //! Reloads an asset (forcefully from the file system).    
    //! \param the asset pointer.
    //! \return the IO error.
    Io::IoError ReloadAsset(Asset* asset);

    //! Destroys the asset called from LoadAsset. Only use this function if LoadAsset is used manually. Calling this
    //! on a runtimeAssetObjectRef will disconnect the object from an asset and set a reference to this asset to null.
    //! \param asset the pointer used to unload.
    void    UnloadAsset(Asset* asset);

    //! Creates a blank asset. If an asset has been loaded, and create asset gets called on it, this function will fail.
    //! This function does not do disc operations, therefore, if you want to save the asset to the disc, call SaveAsset
    //! After creating this, and do proper IO error checkin.
    //! \param name the path to use to create the asset.
    //! \param isStructured wether this asset is structured or not.
    Asset* CreateAsset(const char* path, bool isStructured);

    //! Saves this asset to disc.
    //! \return file io error.
    Io::IoError SaveAsset(Asset* asset);

    /** TODO: implement when required
    //! Copies the asset passed to disc and returns a new copy of it.
    //! \param asset the asset to copy and saveas
    //! \param path the new file path to save the asset to.
    //! \param file io error. if any
    //! \return  a copy of the asset passed. Null if there was an io failure.
    Asset* SaveAssetAs(Asset* asset, const char* path, Io::IoError& outIoError);
    **/

    //-------- High level functions, use this to create / save serializable objects registered through the factories --//

    //! Loads an object. Internally this function calls LoadAsset. This function also searches for the correct
    //! factory representation and the correct type description for the internal asset loaded.
    //! As a first pass it uses the extension to figure out if its structured or not.
    //! It is the callers responsability to cast the ref to the appropiate pointer. The caller could use GetTypeDesc
    //! to verify if this asset is the correct one.
    //! When the refcount of the object return goes to 0, the object gets destroyed, and the asset unloaded automatically.
    //! \param name the file path used to load this object.
    //! \return Null if there was an internal error. Otherwise we return an object
    RuntimeAssetObjectRef LoadObject(const char* path);

    //! Saves a runtime object to disc. The object must have an asset attached to it. This function will fail if
    //! the object does not have an asset attached to it. To attach an asset to an object please use AttachAsset function
    //! inside RuntimeAssetObject.
    Io::IoError SaveObject(RuntimeAssetObjectRef object);

    /** TODO: implement when required
    //! Returns a copy of the object passed in, with an asset attached into the path specified.
    //! \param object the object to copy.
    //! \param the new path to save this object to.
    //! \param outIoError the output IO error of this object.
    RuntimeAssetObjectRef SaveObjectAs(RuntimeAssetObjectIn object, const char* path, Io::IoError& outIoError);
    **/

    //! Creates an object with an asset attached to it. This function calls internally CreateAsset, and attaches it to the object
    //! This function internally uses the factories to instantiate the object. If no object factory is found this asset gets unloaded.
    //! \param the file system path to save the object to.
    //! \param desc the description of the asset type. use the type descriptions from PegasusAssetTypes.h
    RuntimeAssetObjectRef CreateObject(const char* path, const PegasusAssetTypeDesc* desc);

    //! Register an asset runtime object factory. This will be the factory building the object returned in LoadObject function
    //! \param factory - factory to register
    void RegisterObjectFactory(AssetRuntimeFactory* factory);

    //! Sets an alternative IO Manager to handle IO
    void SetIoManager(Io::IOManager* ioMgr) { mIoMgr = ioMgr; }

#if PEGASUS_ENABLE_PROXIES
    IAssetLibProxy* GetProxy() { return &mProxy; }

    //! Reads pure json from a string buffer. Asset can later be saved / or deleted as the regular API.
    //! \param asset the asset to populate
    //! \param buffer the buffer to read json from.
    //! \return bool if successful, false otherwise.
    virtual bool DeserializeAsset(Asset* asset, char* buffer);

    //! Converts an asset to a string. It also does an internal allocation on a string. The string must be then deleted using DestorySerializationString otherwise this memory will leak.
    //! \param asset asset to dump to json
    virtual char* SerializeAsset(Asset* asset);

    //! Destroys the result from SerializeAsset. Must be called always after SerializeAsset once the string result is not used.
    //! \param stringToDelete the string to delete
    virtual void DestroySerializationString(char* stringToDelete);
#endif
    
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    //! Call in the API to start recording categories during loading
    void BeginCategory(Category* category);
    
    //! Call in the API to end recording of categories during loading.
    void EndCategory();

    //! Find the category of assets by type
    Category* FindTypeCategory(const Pegasus::PegasusAssetTypeDesc* typeDesc);

    //! Returns a set of categories.
    const Utils::Vector<Category*>& GetCategories() const { return mCategories; } 

    //! Attempts to remove an asset category.
    bool RemoveAssetCategory(Category* targetToRemove);
#endif


private:
    Pegasus::AssetLib::AssetRuntimeFactory* FindFactory(Asset* asset, const char* ext, const PegasusAssetTypeDesc** outDesc) const;
    Io::IoError InternalBuildAsset(Asset** memory, bool isStructured, const char* path);   

    // resolves any pending child assets
    void ResolvePendingChildAssets(Asset* asset);

#if PEGASUS_ENABLE_PROXIES
    AssetLibProxy mProxy;
#endif
    Alloc::IAllocator* mAllocator;
    AssetBuilder   mBuilder;
    Io::IOManager* mIoMgr;
    Utils::Vector<Asset*> mAssets;
    Utils::Vector<AssetRuntimeFactory*> mFactories;

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    Category* mCurrentCategory;
    Utils::Vector<Category*> mCategories;
    Category* mTypeCategories;
    unsigned mTypeCategoryCount;
#endif
};

}
}

#endif
