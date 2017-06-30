#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Core/Shared/IoErrors.h"
#include "Pegasus/AssetLib/Proxy/RuntimeAssetObjectProxy.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Log.h"


using namespace Pegasus;
using namespace Pegasus::AssetLib;

RuntimeAssetObjectProxy::RuntimeAssetObjectProxy(RuntimeAssetObject* object)
: mObject(object)
{
}

RuntimeAssetObjectProxy::~RuntimeAssetObjectProxy()
{
}


IAssetProxy* RuntimeAssetObjectProxy::GetOwnerAsset() const
{
    if (mObject->GetOwnerAsset() == nullptr) return nullptr;
    return mObject->GetOwnerAsset()->GetProxy();
}

const char* RuntimeAssetObjectProxy::GetDisplayName() const
{
    return mObject->GetDisplayName();
}

void RuntimeAssetObjectProxy::Write(IAssetProxy* asset)
{
    mObject->Write(static_cast<AssetProxy*>(asset)->GetObject());
}

bool RuntimeAssetObjectProxy::ReloadFromAsset()
{
    Asset* asset = mObject->GetOwnerAsset();
    AssetLib* lib = asset->GetLib();

    if (lib->ReloadAsset(asset) != Pegasus::Io::ERR_NONE)
    {
        PG_LOG('ERR_', "Trying to reload an asset (%s) but failed.", asset->GetPath());
        return false;
    }

    mObject->Read(asset);
    return true;
}


#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
