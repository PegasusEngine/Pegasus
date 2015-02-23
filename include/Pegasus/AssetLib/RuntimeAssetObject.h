/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RuntimeAssetObject.h
//! \author Kleber Garcia
//! \date   February 16 2015
//! \brief  Interface that defines what a runtime object, that comes from an asset, looks like

#ifndef PEGASUS_RUNTIME_ASSET_OBJ_H
#define PEGASUS_RUNTIME_ASSET_OBJ_H

namespace Pegasus
{
namespace AssetLib
{

class IAssetFactory;
class AssetLib;
class Asset;

//! Class representing a serializable object
class RuntimeAssetObject
{
public:
    friend class AssetLib;
    friend class Asset;
     
    //! Constructor
    RuntimeAssetObject() : mAsset(nullptr), mTypeId(-1), mAssetLib(nullptr) {}

    //! Destructor
    virtual ~RuntimeAssetObject(){ OnAssetRuntimeDestroyed(); }


private:
    //! Connects an asset to this serializable runtime object
    //! \param asset the asset to connect
    void Connect(Asset* asset, int typeId, AssetLib* assetLib);

    //! Call this when an asset has been destroyed
    void OnAssetRuntimeDestroyed();

    //! Returns the parent asset of this object
    //! \return the parent
    Asset* GetParent() const { return mAsset; }

    //! the asset connected
    Asset* mAsset;

    //! the asset factory used for this runtime object creation
    int mTypeId;

    //! the asset library used for this runtime object actions
    AssetLib* mAssetLib;

    

};

}
}

#endif
