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

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/TimelineProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


Timeline::Timeline(Alloc::IAllocator * allocator)
:   mNumLanes(0),
    mAllocator(allocator)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(allocator, -1, "Timeline::mProxy", Pegasus::Alloc::PG_MEM_PERM) TimelineProxy(this);
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Timeline::~Timeline()
{
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }

#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the timeline
    PG_DELETE(mAllocator, mProxy);
#endif
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
