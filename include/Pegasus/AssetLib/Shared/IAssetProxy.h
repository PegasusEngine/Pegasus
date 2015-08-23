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

namespace Pegasus 
{
struct PegasusAssetTypeDesc;

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
    };
}
}

#endif
