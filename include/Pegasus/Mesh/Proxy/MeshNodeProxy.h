/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus                                           */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshNodeProxy.h
//! \author	Kleber Garcia
//! \date	November 17th, 2015
//! \brief	Mesh node proxy implementation declaration

#ifndef PEGASUS_MESHPROXY_NODE_H
#define PEGASUS_MESHPROXY_NODE_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Mesh/Shared/IMeshNodeProxy.h"

namespace Pegasus {
namespace Mesh {

class Mesh;

class MeshNodeProxy : public IMeshNodeProxy
{
public:
    MeshNodeProxy(Mesh* mesh) :mMesh(mesh) {}
    virtual ~MeshNodeProxy(){}

protected:
    virtual AssetLib::IRuntimeAssetObjectProxy* GetDecoratedObject() const;

private:
    Mesh* mMesh;
};

} // namespace Mesh
} // namespace Pegasus

#endif // PEGASUS_ENABLE_PROXIES
#endif // PEGASUS_MESHPROXY_NODE_H
