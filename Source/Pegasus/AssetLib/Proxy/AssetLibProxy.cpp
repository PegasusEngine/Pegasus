/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLibProxy.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLibProxy Class.

#include "Pegasus/AssetLib/Proxy/AssetLibProxy.h"
#if PEGASUS_ENABLE_PROXIES

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetLibProxy::AssetLibProxy()
    : mAssetLib(nullptr)
{
}

AssetLibProxy::~AssetLibProxy()
{
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif