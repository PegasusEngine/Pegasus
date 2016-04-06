/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IObjectProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects object information

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Proxy/ObjectProxy.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/AssetLib/Asset.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

IAssetProxy* ObjectProxy::GetChildAsset(unsigned i)
{
    return mObject->GetAsset(static_cast<int>(i))->GetOwnerAsset()->GetProxy();
}

unsigned ObjectProxy::GetChildAssetCount()
{
    return static_cast<unsigned>(mObject->GetAssetsCount());
}

IObjectProxy* ObjectProxy::GetChildObject(unsigned i)
{
    return mObject->GetObject(static_cast<int>(i))->GetProxy();
}

unsigned ObjectProxy::GetChildObjectCount()
{
    return static_cast<unsigned>(mObject->GetObjectCount());
}

IArrayProxy* ObjectProxy::GetArray(unsigned i)
{
    return mObject->GetArray(static_cast<int>(i))->GetProxy();
}

unsigned ObjectProxy::GetArrayCount() 
{
    return static_cast<unsigned>(mObject->GetArrayCount());
}


#else

PEGASUS_AVOID_EMPTY_FILE_WARNING;

#endif
