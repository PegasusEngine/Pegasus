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
#include "Pegasus/AssetLib/ASTree.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

void RuntimeAssetObject::Bind(Asset* asset)
{
    mAsset = asset;
    mAsset->SetRuntimeData(this);
}

void RuntimeAssetObject::OnAssetRuntimeDestroyed()
{
    //do something here with the library
    if (mAsset != nullptr)
    {
        mAsset->GetLib()->UnloadAsset(mAsset);
        mAsset = nullptr;
    }
}

bool RuntimeAssetObject::Read(Asset* asset)
{
    AssetLib* lib = asset->GetLib();
    Bind(asset);
    return OnReadAsset(lib, asset);
}

void RuntimeAssetObject::Write(Asset* asset)
{
    asset = asset == nullptr ? GetOwnerAsset() : asset;
    PG_ASSERT(asset != nullptr);
    if (asset != nullptr)
    {
        if (asset->GetFormat() == Asset::FMT_STRUCTURED) 
        {
            asset->Clear();
            Object* obj = asset->NewObject();
            asset->SetRootObject(obj);
            
            obj->AddInt("__type_guid__", asset->GetTypeDesc()->mTypeGuid);
            obj->AddString("typename", asset->GetTypeDesc()->mTypeName);            
                        
        }
        AssetLib* lib = asset->GetLib();
        OnWriteAsset(lib, asset);
    }
}
