/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IAssetLibProxy.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  IAssetLibProxy Interface.

#ifndef PEGASUS_I_ASSET_LIB_PROXY_H
#define PEGASUS_I_ASSET_LIB_PROXY_H

#include "Pegasus/Core/Io.h"

namespace Pegasus
{
namespace AssetLib
{

class IAssetProxy;

//! Asset proxy interface
class IAssetLibProxy
{
public:
    
    //! constructor
    IAssetLibProxy() {}

    //! destructor
    virtual ~IAssetLibProxy() {}

    //! Returns the asset attempting to load
    //! \param path the path to load the asset from
    //! \param assetProxy a pointer to a pointer to fill in the asset if no error occurs
    //! \return IO error in case there was an issue loading the asset file
    virtual Io::IoError LoadAsset(const char* path, IAssetProxy** assetProxy) = 0;

    //! Creates a blank asset. Depending on the extension is either structured or raw.
    //! \param the path to build the blank asset
    //! \param the asset proxy pointer to fill in 
    //! \return IO error in case there was an issue creating the file representing this asset
    virtual Io::IoError CreateBlankAsset(const char* path, IAssetProxy** assetProxy) = 0;

};

}
}

#endif
