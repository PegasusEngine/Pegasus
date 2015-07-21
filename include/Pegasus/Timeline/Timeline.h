/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.h
//! \author	refactored by Kleber Garcia (original from Karolyn Boulanger)
//! \date	July 18, 2015
//! \brief	Timeline container, for lanes, and functions for playback

#ifndef PEGASUS_TIMELINE_TIMELINE_H
#define PEGASUS_TIMELINE_TIMELINE_H

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"

namespace Pegasus {

    namespace Core {
        class IApplicationContext;
    }

    namespace Timeline {
        class Block;
        class Lane;
    }

    namespace Wnd {
        class Window;
    }

    namespace AssetLib {
        class AssetLib;
        class Asset;
    }
}

namespace Pegasus {
namespace Timeline {


//----------------------------------------------------------------------------------------

//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class Timeline : public AssetLib::RuntimeAssetObject
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    Timeline(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~Timeline();

    //! Get the allocator used for all timeline allocations
    inline Alloc::IAllocator * GetAllocator() const { return mAllocator; }

    //! Create a block by class name
    //! \param className Name of the block class to instantiate
    //! \return Pointer to the created block, nullptr if an error occurred
    Block * CreateBlock(const char * className);

    //! Clear the entire timeline and create a default lane
    void Clear();

    //! Set the number of ticks per beat
    //! \param numTicksPerBeat New number of ticks per beat, power of 2, >= 16
    void SetNumTicksPerBeat(unsigned int numTicksPerBeat);

    //! Get the number of ticks per beat
    //! \return Number of ticks per beat, power of 2, >= 16, 128 by default
    inline unsigned int GetNumTicksPerBeat() const { return mNumTicksPerBeat; }

    //! Get the number of ticks per beat in float format
    //! \return Number of ticks per beat in float format, power of 2, >= 16.0f, 128.0f by default
    inline float GetNumTicksPerBeatFloat() const { return mNumTicksPerBeatFloat; }

    //! Get the reciprocal of the number of ticks per beat
    //! \return Reciprocal of the number of ticks per beat
    inline float GetRcpNumTicksPerBeat() const { return mRcpNumTicksPerBeat; }

    //! Set the speed of the timeline in beats per minute
    //! \param bpm Beats per minute (30.0f <= bpm <= 500.0f)
    void SetBeatsPerMinute(float bpm);

    //! Get the speed of the timeline in beats per minute
    //! \return Beats per minute (30.0f <= bpm <= 500.0f)
    inline float GetBeatsPerMinute() const { return mBeatsPerMinute; }


    //! Set the length of the timeline
    //! \warning If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted
    //! \param numBeats Number of beats defining the new length of the timeline (> 0)
    void SetNumBeats(unsigned int numBeats);

    //! Get the length of the timeline
    //! \return Number of beats defining the length of the timeline (>= 1)
    inline unsigned int GetNumBeats() const { return mNumBeats; }


    //! Create a new lane
    //! \return New lane, nullptr if the number of lanes is MAX_NUM_LANES
    Lane * CreateLane();

    //! Get the current number of lanes
    //! \return Number of lanes (>= 1)
    inline unsigned int GetNumLanes() const { return mNumLanes; }

    //! Get one of the lanes
    //! \param laneIndex Index of the lane (< GetNumLanes())
    //! \return Pointer to the lane, nullptr in case of error
    Lane * GetLane(unsigned int laneIndex) const;

    // Tell all the blocks of the timeline to initialize their content (calling their Initialize() function)
    void InitializeBlocks();

    // Tell all the blocks of the timeline to uninitialize their content
    void ShutdownBlocks();

    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    void SetPlayMode(PlayMode playMode);

    //! Update the current state of the timeline based on the play mode and the current time
    //! \param musicPosition Currently heard position of the music (in milliseconds), 0 if unknown
    void Update(unsigned int musicPosition = 0);

    //! Render the content of the timeline for the given window
    //! \param window Window in which the timeline is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    void Render(Wnd::Window * window);

    //! Set the current beat of the timeline
    //! \param beat Current beat, in number of ticks, can have fractional part
    void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, in number of ticks, can have fractional part
    inline float GetCurrentBeat() const { return mCurrentBeat; }

    //! Application context, providing access to the global managers
    //! \return the context
    Core::IApplicationContext* GetApplicationContext() const { return mAppContext; }

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the timeline
    //! \return Proxy associated with the timeline
    //@{
    inline TimelineProxy * GetProxy() { return &mProxy; }
    inline const TimelineProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

protected:


    //! callback to implement reading / parsing an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to read from
    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset);

    //! callback that writes to an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to write to
    virtual void OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset);

    //------------------------------------------------------------------------------------

private:

    // The timeline cannot be copied
    PG_DISABLE_COPY(Timeline)

    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Core::IApplicationContext* mAppContext;

#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the timeline
    TimelineProxy mProxy;

    //! True when the real-time play mode has been selected, to update mStartPegasusTime
    //! on the next call to Update() (used by the editor to set the current beat)
    bool mRequiresStartTimeComputation;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Number of ticks per beat, power of 2, >= 16, 128 by default
    unsigned int mNumTicksPerBeat;

    //! Number of ticks per beat in float format, power of 2, >= 16.0f, 128.0f by default
    float mNumTicksPerBeatFloat;

    //! Reciprocal of the number of ticks per beat
    float mRcpNumTicksPerBeat;

    //! Speed of the timeline in beats per minute (30.0f <= bpm <= 500.0f)
    float mBeatsPerMinute;

    //! Number of beats defining the length of the timeline (>= 1)
    unsigned int mNumBeats;

    //! Set of lanes, only the first mNumLanes are defined
    Lane * mLanes[MAX_NUM_LANES];

    //! Number of used lanes (<= MAX_NUM_LANES)
    unsigned int mNumLanes;

    //! Current play mode of the timeline (PLAYMODE_xxx constant, PLAYMODE_REALTIME by default)
    PlayMode mPlayMode;

    //! Current beat of the timeline, in number of ticks, can have fractional part.
    //! INVALID_BEAT before the first call to Update()
    float mCurrentBeat;

    //! Invalid beat, used to detect the Update() function has not been called yet
    static const float INVALID_BEAT;

    //! Pegasus time returned by \a GetPegasusTime() when the timeline was started being played
    double mStartPegasusTime;

    //! True if the start time has been modified to synchronize the beat of the timeline with the music
    bool mSyncedToMusic;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
