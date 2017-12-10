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

#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Timeline/TimelineScriptRunner.h"
#include "Pegasus/Core/RefCounted.h"

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
#include "Pegasus/AssetLib/Category.h"
#endif

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

    namespace PropertyGrid{
        class PropertyGridObject;
    }

    namespace Sound {
        class Sound;
    }
}

namespace Pegasus {
namespace Timeline {

//! Structure describing an update tick.
struct UpdateInfo
{
    float beat; // the beat number this update requested is
    float relativeBeat;

    UpdateInfo(float pBeat)
    : beat(pBeat)
    , relativeBeat(0.0f) //compute as we pass it down to blocks to recycle memory
    {
    }
};

//! Structure describing
struct RenderInfo
{
    float beat;           // the beat this render request is
    float relativeBeat;   // beat relative to the beginning of this block.
    int windowId;         //the windows id requested
    int viewportWidth;    //the viewport width as an int
    int viewportHeight;   //the viewport height as an int
    float viewportWidthF; //the viewport width as a float
    float viewportHeightF;//the viewport height as a float
    float aspect;         //the aspect ratio (height / width)
    float aspectInv;      // the inverse aspect ratio

    RenderInfo(
        float pBeat
       ,int pWindowId
       ,int pViewportWidth
       ,int pViewportHeight
       ,float pViewportWidthF
       ,float pViewportHeightF
       ,float pAspect
       ,float pAspectInv
       )
     : beat(pBeat)
     ,windowId(pWindowId)
     ,viewportWidth(pViewportWidth)
     ,viewportHeight(pViewportHeight)
     ,viewportWidthF(pViewportWidthF)
     ,viewportHeightF(pViewportHeightF)
     ,aspect(pAspect)
     ,aspectInv(pAspectInv)
     ,relativeBeat(0.0f) //compute as we pass it down to blocks to recycle memory
    {
    }
};

//----------------------------------------------------------------------------------------

//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class Timeline : public Core::RefCounted, public AssetLib::RuntimeAssetObject
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

#if PEGASUS_ENABLE_PROXIES
    //! Deletes a block and shuts it down.
    //! \param blockGuid the block guid to delete.
    //! \return int with the lane it was found. -1 otherwise
    int DeleteBlock(unsigned int blockGuid);
#endif

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
    void Update();

    //! Render the content of the timeline for the given window
    //! \param enumeration of the window index bound for rendering.
    //! \param window - window used to render
    void Render(int windowIndex, Wnd::Window* window);

    //! Set the current beat of the timeline
    //! \param beat Current beat, in number of ticks, can have fractional part
    void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, in number of ticks, can have fractional part
    inline float GetCurrentBeat() const { return mCurrentBeat; }

    //! Application context, providing access to the global managers
    //! \return the context
    Core::IApplicationContext* GetApplicationContext() const { return mAppContext; }

    //! Attempts to open and compile a script. True if success, false otherwise.
    //! \param script reference to attach
    void AttachScript(TimelineScriptInOut script);

    //! Attempts to shutdown a script if it has been opened
    void ShutdownScript();

    //! Attempts to create a music track
    void LoadMusic(const char* path);

    //! Removes a music track.
    void UnloadMusic();

    //! Returns volume scale (0.0 to Max scale).
    //! \returns volume scale
    float GetVolume() const;

    //! Gets volume scale 
    //! \param volume scale
    void SetVolume(float volume);

#if PEGASUS_ENABLE_PROXIES
    //! Enable sound. For mute & unmute.
    void DebugEnableSound(bool enableSound);
#endif

    //! Returns the script of this block, null if none is attached.
    //! \return the script object, null if not attached
    TimelineScriptReturn GetScript();

    //! Returns the script runner of this timeline
    TimelineScriptRunner* GetScriptRunner()
    {
        return &mScriptRunner;
    }

    //! Returns the property grid
    PropertyGrid::PropertyGridObject* GetPropertyGrid()
    {
        return &mPropertyGrid;
    }

#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the timeline
    //! \return Proxy associated with the timeline
    //@{
    inline TimelineProxy * GetProxy() { return &mProxy; }
    inline const TimelineProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES
    
    //! \return the global cache containing resources of this timeline
    Application::GlobalCache* GetGlobalCache() { return &mGlobalCache; }

    //! Callback for when a window is created.
    void OnWindowCreated(int windowIndex);

    //! Callback for when a window is destroyed.
    void OnWindowDestroyed(int windowIndex);

    //------------------------------------------------------------------------------------

protected:


    //! callback to implement reading / parsing an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to read from
    virtual bool OnReadAsset(Pegasus::AssetLib::AssetLib* lib, const AssetLib::Asset* asset) override;

    //! callback that writes to an asset
    //! \param lib the asset library, in case we need to access another asset reference
    //! \param asset the asset to write to
    virtual void OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset) override;

    //------------------------------------------------------------------------------------

private:

    void InternalClear();

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

    //! script helper object
    TimelineScriptRunner mScriptRunner;

    //! True if the start time has been modified to synchronize the beat of the timeline with the music
    bool mSyncedToMusic;

    //! Temporarily hold a property grid to store and interact with dynamic pieces of data.
    PropertyGrid::PropertyGridObject mPropertyGrid;

    //! Global resource cache, containing shared render targets and such.
    Application::GlobalCache mGlobalCache;

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    AssetLib::Category mCategory;
#endif

    Pegasus::Sound::Sound* mMusic;

    //! We keep track of which windows were initialized and which ones werent.
    //! This is to support lazy initialization in case of live editing (for example, if we create a new
    //! timeline, this timeline needs to have calls to OnWindowCreated recorded.
#if PEGASUS_ENABLE_PROXIES
    bool mWindowIsInitialized[PEGASUS_MAX_WORLD_WINDOW_COUNT];
#endif
};

typedef Pegasus::Core::Ref<Timeline> TimelineRef;
typedef Pegasus::Core::Ref<Timeline> & TimelineIn;
typedef Pegasus::Core::Ref<Timeline> & TimelineInOut;
typedef Pegasus::Core::Ref<Timeline> TimelineReturn;


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
