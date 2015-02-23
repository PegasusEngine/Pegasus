/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RuntimeAssetObject.cpp
//! \author Kleber Garcia
//! \date   February 16 2015
//! \brief  Interface that defines what a runtime object, that comes from an asset, looks like


#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

void RuntimeAssetObject::Connect(Asset* asset, int typeId, Pegasus::AssetLib::AssetLib* lib)
{
    mAsset = asset;
    mTypeId = typeId;
    mAssetLib = lib;
}

void RuntimeAssetObject::OnAssetRuntimeDestroyed()
{
    //do something here with the library
    if (mAsset != nullptr)
    {
        mAssetLib->DestroyAsset(mAsset);
        mAsset = nullptr;
    }
}
