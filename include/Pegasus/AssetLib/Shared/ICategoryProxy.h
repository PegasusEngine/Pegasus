/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ICategoryProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects the category information

#ifndef PEGASUS_ASSETLIB_ICATEGORYPROXY_H
#define PEGASUS_ASSETLIB_ICATEGORYPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace AssetLib
{

class IAssetProxy;

class ICategoryProxy
{
public:
    
    //! Constructor
    ICategoryProxy(){}

    //! Destructor 
    virtual ~ICategoryProxy(){}

    //! User data of this category
    virtual int GetUserData() = 0;

    //! Get the asset from the id passed
    virtual IAssetProxy* GetAsset(unsigned i) = 0;
    
    //! Get the count of assets
    virtual unsigned GetAssetCount() const = 0;
};

}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif //PEGASUS_ASSETLIB_ICATEGORYPROXY_H
