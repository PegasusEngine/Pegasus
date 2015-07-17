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

#include "Pegasus/AssetLib/Proxy/RuntimeAssetObjectProxy.h"

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
    RuntimeAssetObject() :
         mAsset(nullptr)
#if PEGASUS_ENABLE_PROXIES
        ,mProxy(this)
#endif
    {
    }

    //! Destructor
    virtual ~RuntimeAssetObject(){ OnAssetRuntimeDestroyed(); }

    //! Returns the parent asset of this object
    //! \return the parent
    Asset* GetOwnerAsset() const { return mAsset; }

    //! Reads the asset being passed
    //! \param asset containing the data to parse
    //! \return success if true
    bool Read(Asset* asset);

    //! Writes to the asset being passed
    //! \param asset containing the data to flush to
    //!        note - if the asset passed is null, then it uses the asset binded.
    void Write(Asset* asset = nullptr);

#if PEGASUS_ENABLE_PROXIES
    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return "Unknown"; }

    RuntimeAssetObjectProxy* GetRuntimeAssetObjectProxy() { return &mProxy; }
#endif

    //! callback to implement reading / parsing an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to read from
    virtual bool OnReadAsset(AssetLib* lib, Asset* asset) = 0;

    //! callback that writes to an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to write to
    virtual void OnWriteAsset(AssetLib* lib, Asset* asset) = 0;

    //! Connects an asset to this serializable runtime object
    //! \param asset the asset to connect
    void Bind(Asset* asset);

private:

    //! Call this when an asset has been destroyed
    void OnAssetRuntimeDestroyed();

    //! the asset connected
    Asset* mAsset;

#if PEGASUS_ENABLE_PROXIES
    RuntimeAssetObjectProxy mProxy;
#endif

};

}
}

#endif
