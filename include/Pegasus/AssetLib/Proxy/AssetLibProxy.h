/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLibProxy.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLibProxy Class.

#ifndef PEGASUS_ASSET_LIB_PROXY_H
#define PEGASUS_ASSET_LIB_PROXY_H
#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Utils/Vector.h"

//fwd declarations
namespace Pegasus
{
    namespace AssetLib
    {
        
        class AssetLib;
    
        class IAssetProxy;
    }
}

namespace Pegasus
{
namespace AssetLib
{

//! Asset library proxy implementation
class AssetLibProxy : public IAssetLibProxy
{
public:

    //! Constructor
    AssetLibProxy(AssetLib* assetLib);

    //! Destructor
    virtual ~AssetLibProxy();

    //! Returns the asset attempting to load
    virtual Io::IoError LoadAsset(const char* path, IAssetProxy** assetProxy);

    //! Saves an asset to disk
    //! \param asset the asset to save
    //! \return the error code
    virtual Io::IoError SaveAsset(IAssetProxy* asset);

    //! Creates a blank asset. If an asset has been loaded, and create asset gets called on it, this function will fail.
    //! This function does not do disc operations, therefore, if you want to save the asset to the disc, call SaveAsset
    //! After creating this, and do proper IO error checkin.
    //! \param name the path to use to create the asset.
    //! \param isStructured wether this asset is structured or not.
    virtual IAssetProxy* CreateAsset(const char* path,  bool isStructured);


    //! Destroys the asset called from LoadAsset. Only use this function if LoadAsset is used manually. Calling this
    //! on a runtimeAssetObjectRef will disconnect the object from an asset and set a reference to this asset to null.
    //! \param asset the pointer used to unload.
    virtual void UnloadAsset(IAssetProxy* asset);

    //! Loads an object. Internally this function calls LoadAsset. This function also searches for the correct
    //! factory representation and the correct type description for the internal asset loaded.
    //! As a first pass it uses the extension to figure out if its structured or not.
    //! It is the callers responsability to cast the ref to the appropiate pointer. The caller could use GetTypeDesc
    //! to verify if this asset is the correct one.
    //! When the refcount of the object return goes to 0, the object gets destroyed, and the asset unloaded automatically.
    //! \param name the file path used to load this object.
    //! \return Null if there was an internal error. Otherwise we return an object
    virtual IRuntimeAssetObjectProxy* LoadObject(const char* path, bool* outIsNew);

    //! Reads pure json from a string buffer. Asset can later be saved / or deleted as the regular API.
    //! \param asset the asset to populate
    //! \param buffer the buffer to read json from.
    //! \return bool if successful, false otherwise.
    virtual bool DeserializeAsset(IAssetProxy* asset, char* buffer);

    //! Converts an asset to a string. It also does an internal allocation on a string. The string must be then deleted using DestorySerializationString otherwise this memory will leak.
    //! \param asset asset to dump to json
    virtual char* SerializeAsset(IAssetProxy* asset);

    //! Destroys the result from SerializeAsset. Must be called always after SerializeAsset once the string result is not used.
    //! \param stringToDelete the string to delete
    virtual void DestroySerializationString(char* stringToDelete);

    //! Saves a runtime object to disc. The object must have an asset attached to it. This function will fail if
    //! the object does not have an asset attached to it. To attach an asset to an object please use AttachAsset function
    //! inside RuntimeAssetObject.
    virtual Io::IoError SaveObject(IRuntimeAssetObjectProxy* object);

    //! Creates an object with an asset attached to it. This function calls internally CreateAsset, and attaches it to the object
    //! This function internally uses the factories to instantiate the object. If no object factory is found this asset gets unloaded.
    //! \param the file system path to save the object to.
    //! \param desc the description of the asset type. use the type descriptions from PegasusAssetTypes.h
    virtual IRuntimeAssetObjectProxy* CreateObject(const char* path, const PegasusAssetTypeDesc* desc);

    //! Closes a refcount of this object
    //! \param object - object to close
    virtual void CloseObject(IRuntimeAssetObjectProxy* object);

    //! Returns the internal object of this proxy
    AssetLib* GetObject() const { return mAssetLib; }

    
    //! Gets the count of categories in the entire app.
    //! category count of assets in the entire app.
    virtual unsigned GetCategoryCount() const;
    
    //! Gets a specific category based on an index.
    //! \param i the index of the category to look for.
    //! \return the category proxy. Cannot be null.
    virtual ICategoryProxy* GetCategory(unsigned i);

    //! Sets the asset event listener on this library manager.
    //! \param listener the listener with callbacks.
    virtual void SetEventListener(IAssetEventListener* listener);

    //! Gets a specific category of a type. These are stored separate from block categories 
    //! \param the type desc to find for this category, if typeDesc is null, we return the untyped assets.
    //! \return the category proxy found. 
    virtual ICategoryProxy* FindTypeCategory(const Pegasus::PegasusAssetTypeDesc* typeDesc); 

    //! Call in the API to start recording categories during loading.
    virtual void BeginCategory(ICategoryProxy* category);
    
    //! Call in the API to end recording of categories during loading.
    virtual void EndCategory();

private:
    AssetLib* mAssetLib;
    Utils::Vector<RuntimeAssetObjectRef> mObjects;
};

}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif
