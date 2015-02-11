/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLib.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLib Class.

#ifndef PEGASUS_ASSET_LIB_H
#define PEGASUS_ASSET_LIB_H

#include "Pegasus/AssetLib/AssetBuilder.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Core/Io.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/AssetLib/Proxy/AssetLibProxy.h"
#endif

//FWD declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace AssetLib
{

class Asset;

//! Asset Library class
class AssetLib
{
public:
    //! Constructor
    //! \param allocator the allocator to use internally
    //! \param ioMgr the input/output mgr, containing logic for the file system
    AssetLib(Alloc::IAllocator* allocator, Io::IOManager* ioMgr);

    //! Destructor
    ~AssetLib();

    //! Loads an asset
    //! \param name the name of this asset
    //! \param the output asset
    Io::IoError LoadAsset(const char* name, Asset** asset);

    //! Saves an asset to disk, using its registered path
    Io::IoError SaveAsset(Asset* asset);

    //! Sets an alternative IO Manager to handle IO
    void SetIoManager(Io::IOManager* ioMgr) { mIoMgr = ioMgr; }

#if PEGASUS_ENABLE_PROXIES
    IAssetLibProxy* GetProxy() { return &mProxy; }
#endif
    

private:
#if PEGASUS_ENABLE_PROXIES
    AssetLibProxy mProxy;
#endif
    Alloc::IAllocator* mAllocator;
    AssetBuilder   mBuilder;
    Io::IOManager* mIoMgr;
    Utils::Vector<Asset*> mAssets;
};

}
}

#endif
