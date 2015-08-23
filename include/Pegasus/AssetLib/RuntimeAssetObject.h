/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   RuntimeAssetObject.h
//! \author Kleber Garcia
//! \date   February 16 2015
//! \brief  Interface that defines what a runtime object, that comes from an asset, looks like

#ifndef PEGASUS_RUNTIME_ASSET_OBJ_H
#define PEGASUS_RUNTIME_ASSET_OBJ_H

#include "Pegasus/AssetLib/Proxy/RuntimeAssetObjectProxy.h"
#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Core/Ref.h"

namespace Pegasus
{

namespace AssetLib
{

class IAssetFactory;
class AssetLib;
class Asset;

//! Class representing a serializable object
class RuntimeAssetObject
{
public:
    friend class AssetLib;
    friend class Asset;
    friend class Core::Ref<RuntimeAssetObject>;
     
    //! Constructor
    explicit RuntimeAssetObject(Core::RefCounted* decoration) :
         mAsset(nullptr)
        ,mDecoration(decoration)
#if PEGASUS_ENABLE_PROXIES
        ,mProxy(this)
#endif
    {
    }

    //! Destructor
    virtual ~RuntimeAssetObject(){ OnAssetRuntimeDestroyed(); }

    //! Returns the parent asset of this object
    //! \return the parent
    Asset* GetOwnerAsset() const { return mAsset; }

    //! Reads the asset being passed
    //! \param asset containing the data to parse
    //! \return success if true
    bool Read(Asset* asset);

    //! Writes to the asset being passed
    //! \param asset containing the data to flush to
    //!        note - if the asset passed is null, then it uses the asset binded.
    void Write(Asset* asset = nullptr);

#if PEGASUS_ENABLE_PROXIES
    //! Returns the display name of this runtime object
    //! \return string representing the display name of this object
    virtual const char* GetDisplayName() const { return "Unknown"; }

    virtual IRuntimeAssetObjectProxy* GetProxy() = 0;
    virtual const IRuntimeAssetObjectProxy* GetProxy() const = 0;

    IRuntimeAssetObjectProxy* GetRuntimeAssetObjectProxy() { return &mProxy; }
#endif

    //! callback to implement reading / parsing an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to read from
    virtual bool OnReadAsset(AssetLib* lib, Asset* asset) = 0;

    //! callback that writes to an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to write to
    virtual void OnWriteAsset(AssetLib* lib, Asset* asset) = 0;

private:

    //! Connects an asset to this serializable runtime object
    //! \param asset the asset to connect
    void Bind(Asset* asset);

    //! increases the refcount on the decoration.
    inline void DecorationIncreaseRefCount() { mDecoration->AddRef(); }

    //! decreases the refcount on the decoration.
    //! \Note: this function could indirectly call the destructor of this object
    inline void DecorationDecreaseRefCount() { mDecoration->Release(); }

    //! Call this when an asset has been destroyed
    void OnAssetRuntimeDestroyed();

    //! the asset connected
    Asset* mAsset;

    Core::RefCounted* mDecoration;

#if PEGASUS_ENABLE_PROXIES
    RuntimeAssetObjectProxy mProxy;
#endif

};

}

namespace Core
{

    //! Specialization of Ref on this class
    
    //! Destructor
    //! \note Decreases the reference counter of the currently pointed object if defined
    Ref<AssetLib::RuntimeAssetObject>::~Ref()
    {
        if (mObject != nullptr)
        {
            mObject->DecorationDecreaseRefCount();
        }
    }
    
    //! Assignment operator with a weak pointer
    //! \param ptr Pointer to copy
    //! \note Decreases the reference counter of the currently pointed object if defined
    //!       and increases the reference counter of the new pointed object if defined
    //! \warning The input pointer is considered as a weak pointer,
    //!          meaning that the object's reference counter should be 0
    Ref<AssetLib::RuntimeAssetObject> & Ref<AssetLib::RuntimeAssetObject>::operator=(AssetLib::RuntimeAssetObject * ptr)
    {
        if (mObject != ptr)
        {
            if (mObject != nullptr)
            {
                mObject->DecorationDecreaseRefCount();
            }
            if (ptr != nullptr)
            {
                ptr->DecorationIncreaseRefCount();
            }
            mObject = ptr;
        }
        return *this;
    }
    
    //! Assignment operator with another reference
    //! \param ref Reference to copy
    //! \note Decreases the reference counter of the currently pointed object if defined
    //!       and increases the reference counter of the new pointed object if defined
    Ref<AssetLib::RuntimeAssetObject> & Ref<AssetLib::RuntimeAssetObject>::operator=(const Ref<AssetLib::RuntimeAssetObject> & ref)
    {
        if (mObject != ref.mObject)
        {
            if (mObject != nullptr)
            {
                mObject->DecorationDecreaseRefCount();
            }
            if (ref.mObject != nullptr)
            {
                ref.mObject->DecorationIncreaseRefCount();
            }
            mObject = ref.mObject;
        }
        return *this;
    }
}

namespace AssetLib
{

//! Reference to a Node, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<RuntimeAssetObject>   RuntimeAssetObjectRef;

//! Const reference to a reference to a Node, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<RuntimeAssetObject> & RuntimeAssetObjectIn;

//! Reference to a reference to a Node, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<RuntimeAssetObject> & RuntimeAssetObjectInOut;

//! Reference to a Node, typically used as the return value of a function
typedef       Pegasus::Core::Ref<RuntimeAssetObject>   RuntimeAssetObjectReturn;

}

}


#endif
