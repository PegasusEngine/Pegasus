/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CategoryProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects the category information

#ifndef PEGASUS_ASSETLIB_CATEGORYPROXY_H
#define PEGASUS_ASSETLIB_CATEGORYPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/ICategoryProxy.h"

namespace Pegasus
{
namespace AssetLib
{

class Category;

class CategoryProxy : public ICategoryProxy
{
public:
    
    CategoryProxy(Category* object) : mObject(object) {}
    virtual ~CategoryProxy() {}

    //! User data of this category
    virtual int GetUserData();

    //! Get the asset from the id passed
    virtual IAssetProxy* GetAsset(unsigned i);
    
    //! Get the count of assets
    virtual unsigned GetAssetCount() const;

    //! Gets the object of this proxy
    Category* GetObject() { return mObject; }
    const Category* GetObject() const { return mObject; }

private:
    Category* mObject;
};

}
}

#endif // PEGASUS_ENABLE_PROXIES
#endif //PEGASUS_ASSETLIB_CATEGORYPROXY_H
