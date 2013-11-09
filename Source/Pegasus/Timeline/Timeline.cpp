/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"

namespace Pegasus {
namespace Timeline {


Timeline::Timeline(Alloc::IAllocator * allocator)
:   mNumLanes(0),
    mAllocator(allocator)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");
}

//----------------------------------------------------------------------------------------

Timeline::~Timeline()
{
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }
}

//----------------------------------------------------------------------------------------

Lane * Timeline::CreateLane()
{
    PG_LOG('TMLN', "Creating a new lane (index %d)", mNumLanes);

    if (mNumLanes < MAX_NUM_LANES)
    {
        mLanes[mNumLanes] = PG_NEW(mAllocator, -1, "Timeline::Lane", Alloc::PG_MEM_PERM) Lane(mAllocator);
        ++mNumLanes;

        return mLanes[mNumLanes - 1];
    }
    else
    {
        PG_FAILSTR("Unable to create a new lane, the maximum number of lane has been reached %d", MAX_NUM_LANES);
        return nullptr;
    }
}


}   // namespace Timeline
}   // namespace Pegasus
