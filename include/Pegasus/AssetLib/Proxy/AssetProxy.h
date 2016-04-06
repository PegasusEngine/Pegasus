/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetProxy.h
//! \author Kleber Garcia 
//! \date   February 18, 2015
//! \brief  Proxy for asset

#ifndef PEGASUS_ASSET_PROXY_H
#define PEGASUS_ASSET_PROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/IAssetProxy.h"

namespace Pegasus 
{
namespace AssetLib
{

    class Asset;

    //! Proxy class interface for assets
    class AssetProxy : public IAssetProxy
    {
    public:

        //! Constructor
        AssetProxy();

        //! Destructor
        virtual ~AssetProxy();
        
        //! Gets the path of this asset
        //! \return the path of this asset
        virtual const char* GetPath() const; 

        //! Gets the type description of this asset
        virtual const PegasusAssetTypeDesc* GetTypeDesc() const;

        //! Return's this asset runtime object representation.
        virtual IRuntimeAssetObjectProxy* GetRuntimeObject() const;

        //! Sets the object of this proxy
        //! \param the asset object to save
        void SetObject(Asset* object) { mObject = object; }

        //! Gets the object of this proxy
        //! \return the asset inside this proxy
        Asset* GetObject() { return mObject; }

        //! Gets the object of this proxy
        //! \return the asset inside this proxy
        const Asset* GetObject() const { return mObject; }

        //! Gets this object's root. returns null if this is not a structured asset.
        virtual IObjectProxy* Root();
             
    private:

        Asset* mObject;
    };
}
}

#endif
#endif
