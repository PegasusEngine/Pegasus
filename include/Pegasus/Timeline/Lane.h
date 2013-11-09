/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Lane.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline lane management, manages a set of blocks on one lane of the timeline

#ifndef PEGASUS_TIMELINE_LANE_H
#define PEGASUS_TIMELINE_LANE_H

namespace Pegasus {
    namespace Timeline {
        class LaneProxy;
    }
}
    
namespace Pegasus {
namespace Timeline {


//! Timeline lane management, manages a set of blocks on one lane of the timeline
class Lane
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    Lane(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~Lane();


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the lane
    inline LaneProxy * GetProxy() const { return mProxy; }

    //! Maximum length of the name for the lane
    enum { MAX_NAME_LENGTH = 31 };

    //! Set the name of the lane
    //! \param name New name of the lane, can be empty or nullptr, but no longer than MAX_NAME_LENGTH
    void SetName(const char * name);

    //! Get the name of the lane
    //! \return Name of the lane, can be empty or nullptr
    inline const char * GetName() const { return mName; }

    //! Test if the name of the lane is defined
    //! \return True if the name is not an empty string
    inline bool IsNameDefined() const { return (mName[0] != '\0'); }

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // Lanes cannot be copied
    PG_DISABLE_COPY(Lane)


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the lane
    LaneProxy * mProxy;

    //! Name of the lane, can be empty or nullptr, but no longer than MAX_NAME_LENGTH
    char mName[MAX_NAME_LENGTH + 1];

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_LANE_H
