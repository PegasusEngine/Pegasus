/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#ifndef PEGASUS_TIMELINE_TIMELINE_H
#define PEGASUS_TIMELINE_TIMELINE_H

namespace Pegasus {
    namespace Timeline {
        class Lane;
    }
}

namespace Pegasus {
namespace Timeline {


//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class Timeline
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    Timeline(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~Timeline();


    //! Maximum number of lanes allowed in the timeline
    enum { MAX_NUM_LANES = 64 };

    //! Create a new lane
    //! \return New lane, nullptr if the number of lanes is MAX_NUM_LANES
    Lane * CreateLane();

    //------------------------------------------------------------------------------------

private:

    // The timeline cannot be copied
    PG_DISABLE_COPY(Timeline)

        
    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Set of lanes, only the first mNumLanes are defined
    Lane * mLanes[MAX_NUM_LANES];

    //! Number of used lanes (<= MAX_NUM_LANES)
    unsigned int mNumLanes;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
