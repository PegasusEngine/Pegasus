/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLibProxy.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLibProxy Class.

#ifndef PEGASUS_ASSET_LIB_PROXY_H
#define PEGASUS_ASSET_LIB_PROXY_H
#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"


//fwd declarations
namespace Pegasus
{
    namespace AssetLib
    {
        class AssetLib;
    }
}

namespace Pegasus
{
namespace AssetLib
{
class AssetLibProxy : public IAssetLibProxy
{
public:
    AssetLibProxy();
    virtual ~AssetLibProxy();

    void SetObject(AssetLib* al) { mAssetLib = al; }

    AssetLib* GetObject() const { return mAssetLib; }

private:
    AssetLib* mAssetLib;
};

}
}

#endif //PEGASUS_ENABLE_PROXIES
#endif
