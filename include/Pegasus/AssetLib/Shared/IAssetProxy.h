/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IAssetProxy.h
//! \author Kleber Garcia 
//! \date   February 18, 2015
//! \brief  Proxy interface for asset

#ifndef PEGASUS_IASSET_PROXY_H
#define PEGASUS_IASSET_PROXY_H

// fwd decl
namespace Pegasus 
{
    struct PegasusAssetTypeDesc;
    
    namespace AssetLib
    {
        class IRuntimeAssetObjectProxy;
        class IObjectProxy;
    }
}

namespace Pegasus 
{
namespace AssetLib
{

//! Proxy class interface for assets
class IAssetProxy
{
public:

    //! Constructor
    IAssetProxy(){}

    //! Destructor
    virtual ~IAssetProxy(){}
    
    //! Gets the path of this asset
    virtual const char* GetPath() const = 0; 

    //! Gets the type description of this asset
    virtual const PegasusAssetTypeDesc* GetTypeDesc() const = 0;

    //! Return's this asset runtime object representation.
    virtual IRuntimeAssetObjectProxy* GetRuntimeObject() const = 0;

    //! Gets this object's root json object. returns null if this is not a structured asset.
    virtual IObjectProxy* Root() = 0;
};
}
}

#endif
