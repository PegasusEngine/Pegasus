/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RuntimeAssetObjectProxy.h
//! \author Kleber Garcia
//! \date   July 11th, 2015
//! \brief  Proxy Interface that defines what a runtime object, that comes from an asset, looks like

#ifndef PEGASUS_RUNTIME_ASSET_OBJ_PROXY_H
#define PEGASUS_RUNTIME_ASSET_OBJ_PROXY_H
#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

namespace Pegasus
{
namespace AssetLib
{

class IAssetProxy;
class RuntimeAssetObject;

//! Class representing a serializable object's proxy
class RuntimeAssetObjectProxy : public IRuntimeAssetObjectProxy
{
public:

    //! Constructor
    RuntimeAssetObjectProxy(RuntimeAssetObject* object);

    //! Destructor
    virtual ~RuntimeAssetObjectProxy();

    //! Returns the parent asset of this object
    //! \return the parent
    virtual IAssetProxy* GetOwnerAsset() const;

    //! Returns the runtime object
    //! \return the runtime object
    RuntimeAssetObject* GetRuntimeAssetObject() const { return mObject; }

    //! Returns the editor name displayed for this asset
    virtual const char* GetDisplayName() const;

    //! Writes/Flushes contents to the target asset passed
    virtual void Write(IAssetProxy* asset);

    //! Reloads changes from the internal asset contained
    //! \return true if successful, false otherwise
    virtual bool ReloadFromAsset();

    //! Gets the property grid proxy on this object if available.
    //! \return propertygrid object if available.
    PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() { return nullptr; }

private:
    RuntimeAssetObject* mObject;
};

}
}

#endif
#endif
