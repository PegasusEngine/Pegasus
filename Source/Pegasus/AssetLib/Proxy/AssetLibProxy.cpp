/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLibProxy.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLibProxy Class.

#include "Pegasus/AssetLib/Proxy/AssetLibProxy.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#if PEGASUS_ENABLE_PROXIES

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetLibProxy::AssetLibProxy()
    : mAssetLib(nullptr)
{
}

AssetLibProxy::~AssetLibProxy()
{
}


Io::IoError AssetLibProxy::LoadAsset(const char* path, IAssetProxy** assetProxy)
{
    *assetProxy = nullptr;
    Asset* asset = nullptr;
    Io::IoError err = mAssetLib->LoadAsset(path, &asset);
    if (asset != nullptr)
    {
        *assetProxy = asset->GetProxy();
    }
    return err;
}

Io::IoError AssetLibProxy::CreateBlankAsset(const char* path, IAssetProxy** assetProxy)
{
    *assetProxy = nullptr;
    Asset* asset = nullptr;
    Io::IoError err = mAssetLib->CreateBlankAsset(path, &asset);
    if (asset != nullptr)
    {
        *assetProxy = asset->GetProxy();
    }
    return err;
}


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
