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


//! Type definition for a position in the timeline (in number of timeline ticks, >= 0)
typedef unsigned int Beat;

//! Type definition for a length in the timeline (in number of timeline ticks, > 0)
typedef unsigned int Duration;


//! Maximum length of a block class name string
enum { MAX_BLOCK_CLASS_NAME_LENGTH = 63 };

#if PEGASUS_ENABLE_PROXIES
//! Maximum length of a block editor string
enum { MAX_BLOCK_EDITOR_STRING_LENGTH = 63 };
#endif

//! Maximum number of blocks that can be registered
enum { MAX_NUM_REGISTERED_BLOCKS = 256 };

//! Maximum number of lanes allowed in the timeline
enum { MAX_NUM_LANES = 64 };


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
