/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IRuntimeAssetObjectProxy.h
//! \author Kleber Garcia
//! \date   July 11th, 2015
//! \brief  Proxy Interface that defines what a runtime object, that comes from an asset, looks like

#ifndef PEGASUS_I_RUNTIME_ASSET_OBJ_PROXY_H
#define PEGASUS_I_RUNTIME_ASSET_OBJ_PROXY_H

//forward declarations
namespace Pegasus
{
    namespace PropertyGrid
    {
        class IPropertyGridObjectProxy;
    }
}

namespace Pegasus
{
namespace AssetLib
{

class IAssetProxy;

//! Class representing a serializable object
class IRuntimeAssetObjectProxy
{
public:

    //! Constructor
    IRuntimeAssetObjectProxy() {}

    //! Destructor
    virtual ~IRuntimeAssetObjectProxy() {}

    //! Returns the parent asset of this object
    //! \return the parent
    virtual IAssetProxy* GetOwnerAsset() const = 0; 

    //! Returns the editor name displayed for this asset
    virtual const char* GetDisplayName() const = 0;

    //! Writes/Flushes contents to the target asset passed
    virtual void Write(IAssetProxy* asset) = 0;

    //! Reloads changes from the internal asset contained
    //! \return true if successful, false otherwise
    virtual bool ReloadFromAsset() = 0;

    //! If this asset runtime object has a property attached, the return it.
    //! \return the property grid object of this proxy. If it doesn't exist then it returns null.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() = 0;

};

//! Use the decorator to avoid reimplementation of RuntimeAssetObjectProxy on proxy objects
//! this allows IShaderProxy, IMeshProxy etc to be casted directly to a IRuntimeAssetObjectProxy
class IRuntimeAssetObjectProxyDecorator : public IRuntimeAssetObjectProxy
{
public:
    //! Constructor
    IRuntimeAssetObjectProxyDecorator() {}

    //! Destructor
    virtual ~IRuntimeAssetObjectProxyDecorator() { }

    //! Returns the parent asset of this object
    //! \return the parent
    virtual IAssetProxy* GetOwnerAsset() const { return GetDecoratedObject()->GetOwnerAsset(); }

    //! Returns the editor name displayed for this asset
    virtual const char* GetDisplayName() const { return GetDecoratedObject()->GetDisplayName(); }

    //! Writes/Flushes contents to the target asset passed
    virtual void Write(IAssetProxy* proxy) { GetDecoratedObject()->Write(proxy); }

    //! Reloads changes from the internal asset contained
    //! \return true if successful, false otherwise
    virtual bool ReloadFromAsset() { return GetDecoratedObject()->ReloadFromAsset(); }

    //! If this asset runtime object has a property attached, the return it.
    //! \return the property grid object of this proxy. If it doesn't exist then it returns null.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() { return GetDecoratedObject()->GetPropertyGrid(); }

protected:
    virtual IRuntimeAssetObjectProxy* GetDecoratedObject() const = 0;


};

}
}

#endif
