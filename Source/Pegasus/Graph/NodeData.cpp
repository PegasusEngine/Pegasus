/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeData.cpp
//! \author	Kevin Boulanger
//! \date	17th October 2013
//! \brief	Base node data class for all graph-based systems (textures, meshes, etc.)

#include "Pegasus/Graph/NodeData.h"

namespace Pegasus {
namespace Graph {


NodeData::NodeData()
:   mRefCount(0),
    mDirty(true)
{
}

//----------------------------------------------------------------------------------------

NodeData::~NodeData()
{
}

//----------------------------------------------------------------------------------------

void NodeData::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        //! \todo Use an allocator
        delete this;
    }
}


}   // namespace Graph
}   // namespace Pegasus