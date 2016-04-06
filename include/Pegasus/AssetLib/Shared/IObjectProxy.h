/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IObjectProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects object information

#ifndef PEGASUS_ASSETLIB_IOBJTPROXY_H
#define PEGASUS_ASSETLIB_IOBJCTPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace AssetLib
{

class IAssetProxy;
class IArrayProxy;

//! Proxy that represents a json object
class IObjectProxy
{
public:

    //! Constructor
    IObjectProxy(){}

    //! Destructor
    virtual ~IObjectProxy(){}

    //! Gets a child asset reference
    virtual IAssetProxy* GetChildAsset(unsigned i) = 0; 

    //! Gets the count of child assets this reference has
    virtual unsigned GetChildAssetCount() = 0;

    //! Gets a child object
    virtual IObjectProxy* GetChildObject(unsigned i) = 0;

    //! Gets the count of the child objects of this object
    virtual unsigned GetChildObjectCount() = 0;

    //! Gets the child array of this object
    virtual IArrayProxy* GetArray(unsigned i) = 0;

    //! Gets the count of the child arrays of this object
    virtual unsigned GetArrayCount() = 0;


};
}
}

#endif
#endif
