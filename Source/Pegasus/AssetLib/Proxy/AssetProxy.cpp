/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetProxy.cpp
//! \author Kleber Garcia 
//! \date   February 18, 2015
//! \brief  Proxy for asset

#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"

#if PEGASUS_ENABLE_PROXIES

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetProxy::AssetProxy()
: mObject(nullptr)
{
}

AssetProxy::~AssetProxy()
{
}

const char* AssetProxy::GetPath() const
{
    return mObject->GetPath();
}

const PegasusAssetTypeDesc* AssetProxy::GetTypeDesc() const
{
    return mObject->GetTypeDesc();
}

IObjectProxy* AssetProxy::Root()
{
    if (mObject->GetFormat() == Asset::FMT_STRUCTURED &&mObject->Root() != nullptr )  
    {
        return mObject->Root()->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

IRuntimeAssetObjectProxy* AssetProxy::GetRuntimeObject() const
{
    if (mObject->GetRuntimeData() != nullptr)
    {
        return mObject->GetRuntimeData()->GetProxy(); 
    }
    return nullptr;
}
#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
