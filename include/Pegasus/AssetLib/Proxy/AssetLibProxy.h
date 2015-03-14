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
    AssetLibProxy();

    //! Destructor
    virtual ~AssetLibProxy();

    //! sets the internal object of this proxy
    void SetObject(AssetLib* al) { mAssetLib = al; }

    //! Returns the asset attempting to load
    virtual Io::IoError LoadAsset(const char* path, IAssetProxy** assetProxy);

    //! Saves an asset to disk
    //! \param asset the asset to save
    //! \return the error code
    virtual Io::IoError SaveAsset(IAssetProxy* asset);

    //! Creates a blank asset. Depending on the extension is either structured or raw.
    //! \param the path to build the blank asset
    //! \param the asset proxy pointer to fill in 
    //! \return IO error in case there was an issue creating the file representing this asset
    virtual Io::IoError CreateBlankAsset(const char* path, IAssetProxy** assetProxy);

    //! Returns the internal object of this proxy
    AssetLib* GetObject() const { return mAssetLib; }

private:
    AssetLib* mAssetLib;
};

}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif
