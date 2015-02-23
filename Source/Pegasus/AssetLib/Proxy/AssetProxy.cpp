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
#include "Pegasus/AssetLib/Asset.h"

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

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING;
#endif
