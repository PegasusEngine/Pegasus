/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetRuntimeFactory.h
//! \author	Kleber Garcia
//! \date	30th July 2015
//! \brief	Factory that generates asset runtime objects

#ifndef ASSET_RUNTIME_FACTORY
#define ASSET_RUNTIME_FACTORY
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Core/Assertion.h"

//fwd declarations
namespace Pegasus
{
    namespace AssetLib
    {
        class Asset;
        class AssetLib;
    }
}

namespace Pegasus
{

namespace AssetLib
{

class AssetRuntimeFactory
{
    friend AssetLib;

public:

    //! Constructor
    //! \param reference to the asset lib
    AssetRuntimeFactory();

    //! Destructor
    virtual ~AssetRuntimeFactory() {}

    //! Creates a runtime object from an asset. This function must add a reference to the 
    //! runtime object returned, (if its ref counted)
    //! \param the asset type requested.
    //! \return the runtime asset created. return null if unsuccessfull.
    virtual RuntimeAssetObjectRef CreateRuntimeObject(const PegasusAssetTypeDesc* desc) = 0;

    //! Returns a null terminated list of asset descriptions this runtime factory will accept.
    //! \return a null terminated list of asset descriptions
    virtual const PegasusAssetTypeDesc*const* GetAssetTypes() const = 0;

    //! \param the asset to query, if its accessible to this factory
    //! \param the extension
    //! \param output pointer with the asset type which was matched by this test. Null if false is returned.
    //! \return true if this asset can be processed by this factory
    bool IsCompatible(const Asset* asset, const char* ext, const Pegasus::PegasusAssetTypeDesc** outputTypeDesc) const;

    //! Gets the asset factory of this manager.
    //! \return assetFactory the asset factory to get
    Pegasus::AssetLib::AssetLib* GetAssetLib() const  { return mAssetLib; }

private:

    //! Only set by the asset lib.
    //! \param assetLib - reference to the asset lib owning this factory.
    void SetAssetLib(Pegasus::AssetLib::AssetLib* assetLib) { PG_ASSERTSTR(mAssetLib == nullptr, "Only one asset lib can own this factory."); mAssetLib = assetLib; }

    Pegasus::AssetLib::AssetLib* mAssetLib; //!> Reference to asset lib.
    
};

}
}

#endif
