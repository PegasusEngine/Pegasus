/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus                                           */
/*                                                                                      */
/****************************************************************************************/

//! \file	IMeshNodeProxy.h
//! \author	Kleber Garcia
//! \date	November 17th, 2015
//! \brief	Mesh node proxy interface

#ifndef PEGASUS_IMESHPROXY_NODE_H
#define PEGASUS_IMESHPROXY_NODE_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

namespace Pegasus {
namespace Mesh {

class IMeshNodeProxy : public AssetLib::IRuntimeAssetObjectProxyDecorator
{
public:
    IMeshNodeProxy(){}
    virtual ~IMeshNodeProxy(){}
};

}
}

#endif // PEGASUS_ENABLE_PROXIES
#endif // PEGASUS_MESHPROXY_NODE_H
