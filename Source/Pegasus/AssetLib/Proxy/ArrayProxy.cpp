/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ArrayProxy.h
//! \author Kleber Garcia
//! \date   January 31st 2016
//! \brief  proxy that reflects json asset array information

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/Proxy/ArrayProxy.h"
#include "Pegasus/AssetLib/ASTree.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;


bool ArrayProxy::IsObjectType() const
{
    return mObj->GetType() == Array::AS_TYPE_OBJECT;
}

bool ArrayProxy::IsAssetType() const
{
    return mObj->GetType() == Array::AS_TYPE_ASSET_PATH_REF;
}

unsigned ArrayProxy::GetSize() const
{
    return (unsigned)mObj->GetSize();
}

IObjectProxy* ArrayProxy::GetObjectEl(unsigned i)
{
    PG_ASSERT(IsObjectType());
    return mObj->GetElement((int)i).o->GetProxy();
}

IAssetProxy* ArrayProxy::GetAssetEl(unsigned i)
{
    PG_ASSERT(IsAssetType());
    return mObj->GetElement((int)i).asset->GetOwnerAsset()->GetProxy();
}
#else

    PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
