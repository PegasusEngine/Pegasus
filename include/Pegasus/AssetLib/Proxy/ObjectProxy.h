/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ObjectProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects object information

#ifndef PEGASUS_ASSETLIB_OBJECTPROXY_H
#define PEGASUS_ASSETLIB_OBJECTPROXY_H

#include "Pegasus/AssetLib/Shared/IObjectProxy.h"

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace AssetLib
{

class Object;

//! Proxy that represents a json object
class ObjectProxy : public IObjectProxy
{
public:

    //! Constructor
    ObjectProxy(Object* object) : mObject(object) {}

    //! Destructor
    virtual ~ObjectProxy(){}

    //! Gets a child asset reference
    virtual IAssetProxy* GetChildAsset(unsigned i); 

    //! Gets the count of child assets this reference has
    virtual unsigned GetChildAssetCount();

    //! Gets a child object
    virtual IObjectProxy* GetChildObject(unsigned i);

    //! Gets the count of the child objects of this object
    virtual unsigned GetChildObjectCount();

    //! Gets the child array of this object
    virtual IArrayProxy* GetArray(unsigned i);

    //! Gets the count of the child arrays of this object
    virtual unsigned GetArrayCount();

private:
    Object* mObject;


};
}
}

#endif
#endif
