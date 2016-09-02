/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeData.cpp
//! \author	Karolyn Boulanger
//! \date	17th October 2013
//! \brief	Base node data class for all graph-based systems (textures, meshes, etc.)

#include "Pegasus/Graph/NodeData.h"

namespace Pegasus {
namespace Graph {


NodeData::NodeData(Alloc::IAllocator * allocator)
:   mAllocator(allocator),
    mRefCount(0),
    mNodeGPUData(nullptr),
    mDirty(true),
    mGPUDataDirty(true)
{
}

//----------------------------------------------------------------------------------------

NodeData::~NodeData()
{
    PG_ASSERTSTR(mNodeGPUData == nullptr, "GPU data not freed! this means there is a memory leak.");
}

//----------------------------------------------------------------------------------------

void NodeData::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        //! \todo The destructor is called explicitly here because PG_DELETE does not do it.
        //!       This should be replaced by implicit destructors
        this->~NodeData();
        PG_DELETE(mAllocator, this);
    }
}


}   // namespace Graph
}   // namespace Pegasus
