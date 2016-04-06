/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IArrayProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects json asset array information

#ifndef PEGASUS_ASSETLIB_IARRPROXY_H
#define PEGASUS_ASSETLIB_IARRPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace AssetLib
{

class IObjectProxy;
class IAssetProxy;

//! Proxy that represents a json array
class IArrayProxy
{
public:

    //! Constructor
    IArrayProxy(){}

    //! Destructor
    virtual ~IArrayProxy(){}

    //! Returns true if this array holds objects
    virtual bool IsObjectType() const = 0;

    //! Returns true if this array holds objects
    virtual bool IsAssetType() const = 0;

    //! Returns the size of this array
    virtual unsigned GetSize() const = 0;

    //! Returns the element of this array
    virtual IObjectProxy* GetObjectEl(unsigned i) = 0;

    //! Returns the element of this array
    virtual IAssetProxy* GetAssetEl(unsigned i) = 0;

};
}
}

#endif
#endif
