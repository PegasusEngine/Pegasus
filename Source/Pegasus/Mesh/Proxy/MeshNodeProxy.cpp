//! \file	MeshNodeProxy.cpp
//! \author	Kleber Garcia
//! \date	November 17th, 2015
//! \brief	Mesh node proxy implementation

#include "Pegasus/Mesh/Proxy/MeshNodeProxy.h"
#include "Pegasus/Mesh/Mesh.h"

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Mesh {

AssetLib::IRuntimeAssetObjectProxy* MeshNodeProxy::GetDecoratedObject() const
{
    return mMesh->GetRuntimeAssetObjectProxy();
}

}
}

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif

