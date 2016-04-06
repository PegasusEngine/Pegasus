/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Category.h
//! \author Kleber Garcia
//! \date   26th January 2016
//! \brief  Asset Category file. Represents a category of assets.
//!         Categories are used to partition assets, and to browse them through proxies.
//!         They are compiled out because they serve no other functional purpose other than referencing assets.

#ifndef PEGASUS_AS_CATEGORY_H
#define PEGASUS_AS_CATEGORY_H

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES

#include "Pegasus/Utils/Vector.h"
#include "Pegasus/AssetLib/Proxy/CategoryProxy.h"

namespace Pegasus
{
namespace AssetLib
{

//! fwd declarations
class Asset;


class Category
{
public:

    //! Constructor
    Category();

    //! Destructor
    ~Category();

    //! User data value
    unsigned GetUserData() const { return mUserData; }

    //! User data value setter
    void SetUserData(unsigned userData) { mUserData =  userData; }

    //! Register an asset
    void RegisterAsset(Asset* asset);

    //! Unregister an asset
    //! \param asset the asset to unregister
    //! \param calledFromAssetDestructor only true if called from an asset being unloaded completely.
    void UnregisterAsset(Asset* asset, bool calledFromAssetDestructor = false);

    //! Get the asset from the id passed
    Asset* GetAsset(unsigned i) const { return mAssets[i]; }

    //! Get the count of assets
    unsigned GetAssetCount() const { return mAssets.GetSize(); }

    //! Clears this category. 
    void RemoveAssets();

#if PEGASUS_ENABLE_PROXIES
    ICategoryProxy* GetProxy() { return &mProxy; }
#endif

private:
    unsigned mUserData;
    Utils::Vector<Asset*> mAssets;

#if PEGASUS_ENABLE_PROXIES
    CategoryProxy mProxy;
#endif
};

}
}

#endif //if PEGASUS_ASSETLIB_ENABLE_CATEGORIES

#endif //PEGASUS_AS_CATEGORY_H
