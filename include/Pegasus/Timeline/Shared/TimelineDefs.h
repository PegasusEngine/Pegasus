/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineDefs.h
//! \author	Kevin Boulanger
//! \date	25th December 2013
//! \brief	Type definitions for the timeline

#ifndef PEGASUS_TEXTURE_SHARED_TIMELINEDEFS_H
#define PEGASUS_TEXTURE_SHARED_TIMELINEDEFS_H


namespace Pegasus {
    namespace Timeline {


//! Available play modes for the timeline
enum PlayMode
{
    PLAYMODE_REALTIME,      //!< Updates the beat from the current system time when calling \a Update().
    PLAYMODE_STOPPED,       //!< Does not change the current beat when calling \a Update(). Requires \a SetCurrentBeat().

    NUM_PLAYMODES
};


    }   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_SHARED_TIMELINEDEFS_H
