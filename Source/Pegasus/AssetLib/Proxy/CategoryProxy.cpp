/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CategoryProxy.cpp
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects the category information

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Proxy/CategoryProxy.h"
#include "Pegasus/AssetLib/Category.h"
#include "Pegasus/AssetLib/Asset.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

int CategoryProxy::GetUserData()
{
    return mObject->GetUserData();
}

IAssetProxy* CategoryProxy::GetAsset(unsigned i)
{
    return mObject->GetAsset(i)->GetProxy();
}

unsigned CategoryProxy::GetAssetCount() const
{
    return mObject->GetAssetCount();
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING;

#endif
