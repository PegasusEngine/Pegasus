/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ArrayProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects json asset array information

#ifndef PEGASUS_ASSETLIB_ARRPROXY_H
#define PEGASUS_ASSETLIB_ARRPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/IArrayProxy.h"

namespace Pegasus
{
namespace AssetLib
{

class IObjectProxy;
class Array;

//! Proxy that represents a json array
class ArrayProxy : public IArrayProxy
{
public:

    //! Constructor
    explicit ArrayProxy(Array* arr) : mObj(arr) {}

    //! Destructor
    virtual ~ArrayProxy(){}

    //! Returns true if this array holds objects
    virtual bool IsAssetType() const;

    //! Returns true if this array holds objects
    virtual bool IsObjectType() const;

    //! Returns the size of this array
    virtual unsigned GetSize() const;

    //! Returns the element of this array
    virtual IObjectProxy* GetObjectEl(unsigned i);

    //! Returns the element of this array
    virtual IAssetProxy* GetAssetEl(unsigned i);

private:

    Array* mObj;

};
}
}

#endif
#endif
