/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Category.cpp
//! \author Kleber Garcia
//! \date   26th January 2016
//! \brief  Asset Category file. Represents a category of assets.
//!         Categories are used to partition assets, and to browse them through proxies.
//!         They are compiled out because they serve no other functional purpose other than referencing assets.

#include "Pegasus/AssetLib/Category.h"
#include "Pegasus/AssetLib/Asset.h"

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES

using namespace Pegasus;
using namespace Pegasus::AssetLib;

Category::Category()
: mUserData(0)
#if PEGASUS_ENABLE_PROXIES
, mProxy(this)
#endif
{
}

Category::~Category()
{
    RemoveAssets();
}

void Category::RegisterAsset(Asset* asset)
{
    for (unsigned i = 0; i < mAssets.GetSize(); ++i) if (asset == mAssets[i]) return;
    mAssets.PushEmpty() = asset;
    asset->RegisterToCategory(this);
}

void Category::UnregisterAsset(Asset* asset, bool calledFromAssetDestructor)
{
    if (!calledFromAssetDestructor)
        asset->UnregisterToCategory(this);

    for (unsigned i = 0; i < mAssets.GetSize(); ++i)
    {
        if (asset == mAssets[i])
        {
            mAssets.Delete(i);
            return;
        }
    }
    PG_FAILSTR("Can't find asset to deregister.");
}

void Category::RemoveAssets()
{
    for (unsigned i = 0; i < mAssets.GetSize(); ++i)
    {
        mAssets[i]->UnregisterToCategory(this);
    }

    mAssets.Clear();
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
