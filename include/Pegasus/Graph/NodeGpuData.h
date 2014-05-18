/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeGPUData.h
//! \author Kleber Garcia	
//! \date	23rd March 2014
//! \brief	GPU Data base structure. Contains internal utilities to do type checking casting
//!         during dev mode. A render package will utilize this RTTI system to do safe castings
//!         For performance and code size, this struct does unsafe casts and avoids usage of c++
//!         polymorphism on purpose (hence the usage of a structure).
//!         WARNING!! do not declare constructor on custom GPU data. initialize data in respective
//!                   factory functions

#ifndef PEGASUS_GRAPH_NODEGPUDATA_H
#define PEGASUS_GRAPH_NODEGPUDATA_H

#if PEGASUS_GRAPH_GPUDATA_RTTI

//! utility macro that performs a safecast of a GPU data pointer
#define PEGASUS_GRAPH_GPUDATA_SAFECAST(type, object) reinterpret_cast<type*>(object);PG_ASSERTSTR(type::__GUID == reinterpret_cast<type*>(object)->__private_GUID, "invalid cast!");

//! utility macro that registers rtti guid for custom GPU data
//! always needs to go as the first statement of the custom GPU data struct
#define PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(className, guid) \
    className() \
    : __private_GUID(guid) {} \
    static const int __GUID = guid; \
    int __private_GUID;

#else

//! utility macro that performs a safecast of a GPU data pointer
#define PEGASUS_GRAPH_GPUDATA_SAFECAST(type, object) reinterpret_cast<type*>(object)

//! utility macro that should be the first statement in the declaration of a custom node GPU data structure
//! always needs to go as the first statement of the custom GPU data struct
#define PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(className, guid) className(){}

#endif

namespace Pegasus {
namespace Graph {

struct NodeGPUData
{
PEGASUS_GRAPH_REGISTER_GPUDATA_RTTI(NodeGPUData, 0x0)
};

}
}

#endif
